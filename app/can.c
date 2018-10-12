#include "can.h"
#include "canaddr.h"
#include "data.h"
#include "vx.h"

#ifndef _ASMLANGUAGE
IMPORT unsigned char sysInumTbl[];
#endif /* _ASMLANGUAGE */

#define PELI_MODE   0x00
#define PELI_CMR    0x01
#define PELI_SR     0x02
#define PELI_IR     0x03
#define PELI_IER    0x04
#define PELI_BTR(x) (0x06 + 0x01 * x)
#define PELI_OCR    0x08
#define PELI_ALC    0x0B
#define PELI_ECC    0x0C
#define PELI_EWLR   0x0D
#define PELI_RXERR  0x0E
#define PELI_TXERR  0x0F
#define PELI_TXB(x) (0x10 + 0x01 * x)
#define PELI_RXB(x) (0x10 + 0x01 * x)
#define PELI_ACR(x) (0x10 + 0x01 * x)
#define PELI_AMR(x) (0x14 + 0x01 * x)
#define PELI_RMC    0x1D
#define PELI_RBSA   0x1E
#define PELI_CDR    0x1F

#define WRITE_BYTE(addr, data)         \
{                                      \
        *(unsigned char *)addr = data; \
}

#define WRITE_REG_BYTE(addr, reg, data)                          \
{                                                                \
        *(unsigned char *)addr = (unsigned char)reg;             \
        *(unsigned char *)(addr + 0x1000) = (unsigned char)data; \
}

#define READ_REG_BYTE(addr, reg)                     \
({                                                   \
        *(unsigned char *)addr = (unsigned char)reg; \
        *(unsigned char *)(addr + 0x1000);           \
})

extern int tid_can;
extern MSG_Q_ID msg_can[8][3];
extern MSG_Q_ID msg_tls;
extern MSG_Q_ID msg_vsl;
extern MSG_Q_ID msg_psu;
extern MSG_Q_ID msg_swh;
extern MSG_Q_ID msg_rse;
extern MSG_Q_ID msg_swv;
extern MSG_Q_ID msg_prp;
extern MSG_Q_ID msg_xyz;
extern MSG_Q_ID msg_shd;
extern MSG_Q_ID msg_mom;
extern MSG_Q_ID msg_gen;
extern struct data sys_data;
extern struct ecu sys_ecu[256];

static void isr_rx_can0(int addr); /* CAN总线0接收中断服务程序 */
static void isr_rx_can1(int addr); /* CAN总线1接收中断服务程序 */
static void init_can(int *addr, int *irq, int cable); /* 初始化CAN总线 */
static struct ext *p_can_rx[8]; /* CAN总线接收缓冲链表游标 */

void t_can(int addr, int irq, int n, int period, int duration)
{
        int i, j; /* 循环变量 */
        unsigned char id[4]; /* 中间变量存储ID（29位ID左对齐）*/
        unsigned delta; /* 缓冲链表累积时间差 */
        struct ext buf[8][duration]; /* CAN总线缓存数据及存储数量*/
        struct {
                int tid; /* 数据来源的任务ID */
                struct ext *p; /* 接收数据的首地址 */
        } recv;
        LIST lst[8]; /* 定义链表头 */
        for (i = 0; i < n; i++) {
           /* 开始：构建接收缓冲链表 */
                lstInit(&lst[i]);
                for (j = 0; j < duration; j++)
                  lstAdd(&lst[i], (NODE *)&buf[i][j]);
                lstFirst(&lst[i])->previous = lstLast(&lst[i]);
                lstLast(&lst[i])->next = lstFirst(&lst[i]);
                p_can_rx[i] = (struct ext *)lstFirst(&lst[i]);
                 /* 结束：构建接收缓冲链表 */
                init_can((int *)addr, (int *)irq, i); /* 硬件初始化 */
        }
        for (;;) {
                taskDelay(period); /* 延时 */
                for (i = 0; i < n; i++) {
                        if (READ_REG_BYTE(ADDR(i), PELI_SR) & 0x80) { /* 状态寄存器异常 */
                                init_can((int *)addr, (int *)irq, i); /* 重新初始化硬件 */
                                continue;
                        }
                        if (8 == msgQReceive(msg_can[i][0], (char *)&recv, 8, NO_WAIT) ||
                            8 == msgQReceive(msg_can[i][1], (char *)&recv, 8, NO_WAIT) ||
                            8 == msgQReceive(msg_can[i][2], (char *)&recv, 8, NO_WAIT)) { /* 消息队列非空 */
                                *(unsigned *)id = *(unsigned *)recv.p->id << 3; /* 29位ID左对齐 */
                                 /* 开始：向总线发送数据 */
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(0), CAN_FF);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(1), recv.p->id[3]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(2), recv.p->id[2]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(3), recv.p->id[1]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(4), recv.p->id[0]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(5), recv.p->data[0]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(6), recv.p->data[1]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(7), recv.p->data[2]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(8), recv.p->data[3]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(9), recv.p->data[4]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(10), recv.p->data[5]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(11), recv.p->data[6]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(12), recv.p->data[7]);
                                WRITE_REG_BYTE(ADDR(i), PELI_CMR, 0x01);
                                 /* 结束：向总线发送数据 */
                        }
                        if (((struct ext *)lstPrevious((NODE *)p_can_rx[i]))->ts && p_can_rx[i]->ts) { /* 缓冲链表已满 */
                                delta = ((struct ext *)lstPrevious((NODE *)p_can_rx[i]))->ts - p_can_rx[i]->ts;
                                if (delta < duration - 10) { /* 缓冲累积时间小于10s */
                                        switch (i) {
                                        case 0:
                                                sys_data.fault.bus0 = 1; /* 总线0负载率过高 */
                                                break;
                                        case 1:
                                                sys_data.fault.bus1 = 1; /* 总线1负载录过高 */
                                                break;
                                        default:
                                                break;
                                        }
                                } else if (delta > duration + 10) { /* 缓冲累积时间大于10s */
                                        switch (i) {
                                        case 0:
                                                sys_data.fault.bus0 = 0; /* 总线0负载率正常 */
                                                break;
                                        case 1:
                                                sys_data.fault.bus1 = 0; /* 总线1负载率正常 */
                                                break;
                                        default:
                                                break;
                                        }
                                }
                        } else {
                                        switch (i) {
                                        case 0:
                                                sys_data.fault.bus0 = 0; /* 总线0负载率正常 */
                                                break;
                                        case 1:
                                                sys_data.fault.bus1 = 0; /* 总线1负载率正常 */
                                                break;
                                        default:
                                                break;
                                        }
                        }
                }
        }
}

static void isr_rx_can0(int addr)
{
        struct {
                int tid; /* 自身任务ID */
                struct ext *p; /* 接收数据首地址 */
        } send;
        send.tid = tid_can;
        if (READ_REG_BYTE(addr, PELI_IR) != 0x01 || /* 中断寄存器异常 */
            (READ_REG_BYTE(addr, PELI_SR) & 0x03) != 0x01 || /* 状态寄存器异常 */
            READ_REG_BYTE(addr, PELI_RXB(0)) != CAN_FF) { /* 不是扩展帧 */
                WRITE_REG_BYTE(addr, PELI_CMR, 0x04); /* 释放接收缓冲器 */
                return;
        }
         /* 开始：接收数据并拷贝到缓冲链表中 */
        p_can_rx[0]->id[3] = READ_REG_BYTE(addr, PELI_RXB(1));
        p_can_rx[0]->id[2] = READ_REG_BYTE(addr, PELI_RXB(2));
        p_can_rx[0]->id[1] = READ_REG_BYTE(addr, PELI_RXB(3));
        p_can_rx[0]->id[0] = READ_REG_BYTE(addr, PELI_RXB(4));
        p_can_rx[0]->data[0] = READ_REG_BYTE(addr, PELI_RXB(5));
        p_can_rx[0]->data[1] = READ_REG_BYTE(addr, PELI_RXB(6));
        p_can_rx[0]->data[2] = READ_REG_BYTE(addr, PELI_RXB(7));
        p_can_rx[0]->data[3] = READ_REG_BYTE(addr, PELI_RXB(8));
        p_can_rx[0]->data[4] = READ_REG_BYTE(addr, PELI_RXB(9));
        p_can_rx[0]->data[5] = READ_REG_BYTE(addr, PELI_RXB(10));
        p_can_rx[0]->data[6] = READ_REG_BYTE(addr, PELI_RXB(11));
        p_can_rx[0]->data[7] = READ_REG_BYTE(addr, PELI_RXB(12));
         /* 结束：接收数据并拷贝到缓冲链表中 */
        *(unsigned *)p_can_rx[0]->id >>= 3; /* 29位ID左对齐 */
        if (p_can_rx[0]->id[1] == CA_MAIN && sys_ecu[p_can_rx[0]->id[0]].msg) { /* 收到的ID和数据正确 */
                p_can_rx[0]->ts = tickGet(); /* 打上时标 */
                send.p = p_can_rx[0];
                msgQSend(sys_ecu[p_can_rx[0]->id[0]].msg, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL); /* 发送到相应的消息队列 */
                p_can_rx[0] = (struct ext *)lstNext((NODE *)p_can_rx[0]); /* 游标指向下一个节点 */
        }
        WRITE_REG_BYTE(addr, PELI_CMR, 0x04); /* 释放接收缓冲器 */
}

static void isr_rx_can1(int addr)
{
        struct {
                int tid;
                struct ext *p;
        } send;
        send.tid = tid_can;
        if (READ_REG_BYTE(addr, PELI_IR) != 0x01 ||
            (READ_REG_BYTE(addr, PELI_SR) & 0x03) != 0x01 ||
            READ_REG_BYTE(addr, PELI_RXB(0)) != CAN_FF) {
                WRITE_REG_BYTE(addr, PELI_CMR, 0x04);
                return;
        }
        p_can_rx[1]->id[3] = READ_REG_BYTE(addr, PELI_RXB(1));
        p_can_rx[1]->id[2] = READ_REG_BYTE(addr, PELI_RXB(2));
        p_can_rx[1]->id[1] = READ_REG_BYTE(addr, PELI_RXB(3));
        p_can_rx[1]->id[0] = READ_REG_BYTE(addr, PELI_RXB(4));
        p_can_rx[1]->data[0] = READ_REG_BYTE(addr, PELI_RXB(5));
        p_can_rx[1]->data[1] = READ_REG_BYTE(addr, PELI_RXB(6));
        p_can_rx[1]->data[2] = READ_REG_BYTE(addr, PELI_RXB(7));
        p_can_rx[1]->data[3] = READ_REG_BYTE(addr, PELI_RXB(8));
        p_can_rx[1]->data[4] = READ_REG_BYTE(addr, PELI_RXB(9));
        p_can_rx[1]->data[5] = READ_REG_BYTE(addr, PELI_RXB(10));
        p_can_rx[1]->data[6] = READ_REG_BYTE(addr, PELI_RXB(11));
        p_can_rx[1]->data[7] = READ_REG_BYTE(addr, PELI_RXB(12));
        *(unsigned *)p_can_rx[1]->id >>= 3;
        if (p_can_rx[1]->id[1] == CA_MAIN && sys_ecu[p_can_rx[1]->id[0]].msg) {
                p_can_rx[1]->ts = tickGet();
                send.p = p_can_rx[1];
                msgQSend(sys_ecu[p_can_rx[1]->id[0]].msg, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                p_can_rx[1] = (struct ext *)lstNext((NODE *)p_can_rx[1]);
        }
        WRITE_REG_BYTE(addr, PELI_CMR, 0x04);
}

static void init_can(int *addr, int *irq, int cable)
{
        void (*isr[8])(int) = {isr_rx_can0, isr_rx_can1}; /* CAN总线接收中断服务程序函数指针 */
        sysIntDisablePIC(irq[cable]); /* 关中断 */
         /* 开始：硬件初始化 */
        WRITE_REG_BYTE(addr[cable], PELI_MODE, 0x09);
        WRITE_REG_BYTE(addr[cable], PELI_CMR, 0x0C);
        WRITE_REG_BYTE(addr[cable], PELI_CDR, 0x88);
        WRITE_REG_BYTE(addr[cable], PELI_IER, 0x09);
        WRITE_REG_BYTE(addr[cable], PELI_ACR(0), 0xFF);
        WRITE_REG_BYTE(addr[cable], PELI_ACR(1), 0xFF);
        WRITE_REG_BYTE(addr[cable], PELI_ACR(2), 0xFF);
        WRITE_REG_BYTE(addr[cable], PELI_ACR(3), 0xFF);
        WRITE_REG_BYTE(addr[cable], PELI_AMR(0), 0xFF);
        WRITE_REG_BYTE(addr[cable], PELI_AMR(1), 0xFF);
        WRITE_REG_BYTE(addr[cable], PELI_AMR(2), 0xFF);
        WRITE_REG_BYTE(addr[cable], PELI_AMR(3), 0xFF);
        WRITE_REG_BYTE(addr[cable], PELI_BTR(0), 0x04);
        WRITE_REG_BYTE(addr[cable], PELI_BTR(1), 0x1C);
        WRITE_REG_BYTE(addr[cable], PELI_EWLR, 0x60);
        WRITE_REG_BYTE(addr[cable], PELI_OCR, 0x1A);
        WRITE_REG_BYTE(addr[cable], PELI_MODE, 0x08);
         /* 结束：硬件初始化 */
        intConnect(INUM_TO_IVEC(sysInumTbl[irq[cable]]), (VOIDFUNCPTR)isr[cable], addr[cable]); /* 绑定中断 */
        sysIntEnablePIC(irq[cable]); /* 开中断 */
}

/* 波特率配置表
  +-----------------------+
  | BPS     | BTR0 | BTR1 |
  +---------+------+------+
  | 5Kbps   | 0xBF | 0xFF |
  | 10Kbps  | 0x31 | 0x1C |
  | 20Kbps  | 0x18 | 0x1C |
  | 40Kbps  | 0x87 | 0xFF |
  | 50Kbps  | 0x09 | 0x1C |
  | 80Kbps  | 0x83 | 0Xff |
  | 100Kbps | 0x04 | 0x1C |
  | 125Kbps | 0x03 | 0x1C |
  | 200Kbps | 0x81 | 0xFA |
  | 250Kbps | 0x01 | 0x1C |
  | 400Kbps | 0x80 | 0xFA |
  | 500Kbps | 0x00 | 0x1C |
  | 666Kbps | 0x80 | 0xB6 |
  | 800Kbps | 0x00 | 0x16 |
  | 1Mbps   | 0x00 | 0x14 |
  +---------+------+------+
*/
