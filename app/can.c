#include "can.h"
#include "canaddr.h"
#include "type.h"
#include "vx.h"

#ifndef _ASMLANGUAGE
IMPORT u8 sysInumTbl[];
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

#define ADDR(x) (0xD1000 + 0x2000 * x)

#define WRITE_BYTE(addr, data) \
{                              \
        *(u8 *)addr = data;    \
}

#define WRITE_REG_BYTE(addr, reg, data)    \
{                                          \
        *(u8 *)addr = (u8)reg;             \
        *(u8 *)(addr + 0x1000) = (u8)data; \
}

#define READ_REG_BYTE(addr, reg) \
({                               \
        *(u8 *)addr = (u8)reg;   \
        *(u8 *)(addr + 0x1000);  \
})

extern int tid_can;
extern MSG_Q_ID msg_can[2][3];
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
extern ECU sys_ecu[256];

static void isr_rx_can0(void);
static void isr_rx_can1(void);
static void init_can0(void);
static void init_can1(void);
static CAN buf_can[2][200];
static CAN *p_can[2];
static LIST lst_can[2];

void t_can(int period)
{
        int recv[2];
        int i;
        u8 id[4];
        CAN *p;
        lstInit(&lst_can[0]);
        lstInit(&lst_can[1]);
        for (i = 0; i < 200; i++) {
                lstAdd(&lst_can[0], (NODE *)&buf_can[0][i]);
                lstAdd(&lst_can[1], (NODE *)&buf_can[1][i]);
        }
        lstFirst(&lst_can[0])->previous = lstLast(&lst_can[0]);
        lstFirst(&lst_can[1])->previous = lstLast(&lst_can[1]);
        lstLast(&lst_can[0])->next = lstFirst(&lst_can[0]);
        lstLast(&lst_can[1])->next = lstFirst(&lst_can[1]);
        p_can[0] = (CAN *)lstFirst(&lst_can[0]);
        p_can[1] = (CAN *)lstFirst(&lst_can[1]);
        init_can0();
        init_can1();
        for (;;) {
                taskDelay(period);
                for (i = 0; i < 2; i++) {
                        if (READ_REG_BYTE(ADDR(i), PELI_SR) & 0x80) {
                                if (!i)
                                        init_can0();
                                else
                                        init_can1();
                                continue;
                        }
                        if (8 == msgQReceive(msg_can[i][0], (str)recv, 8, NO_WAIT) ||
                            8 == msgQReceive(msg_can[i][1], (str)recv, 8, NO_WAIT) ||
                            8 == msgQReceive(msg_can[i][2], (str)recv, 8, NO_WAIT)) {
                                p = (CAN *)recv[1];
                                *(u32 *)id = *(u32 *)p->id << 3;
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(0), CAN_FF);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(1), p->id[3]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(2), p->id[2]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(3), p->id[1]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(4), p->id[0]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(5), p->data[0]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(6), p->data[1]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(7), p->data[2]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(8), p->data[3]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(9), p->data[4]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(10), p->data[5]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(11), p->data[6]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(12), p->data[7]);
                                WRITE_REG_BYTE(ADDR(i), PELI_CMR, 0x01);
                        }
                }
        }
}

static void isr_rx_can0(void)
{
        int send[2] = {tid_can, 0};
        if (READ_REG_BYTE(ADDR(0), PELI_IR) != 0x01 ||
            (READ_REG_BYTE(ADDR(0), PELI_SR) & 0x03) != 0x01 ||
            READ_REG_BYTE(ADDR(0), PELI_RXB(0)) != CAN_FF) {
                WRITE_REG_BYTE(ADDR(0), PELI_CMR, 0x04);
                return;
        }
        p_can[0]->id[3] = READ_REG_BYTE(ADDR(0), PELI_RXB(1));
        p_can[0]->id[2] = READ_REG_BYTE(ADDR(0), PELI_RXB(2));
        p_can[0]->id[1] = READ_REG_BYTE(ADDR(0), PELI_RXB(3));
        p_can[0]->id[0] = READ_REG_BYTE(ADDR(0), PELI_RXB(4));
        p_can[0]->data[0] = READ_REG_BYTE(ADDR(0), PELI_RXB(5));
        p_can[0]->data[1] = READ_REG_BYTE(ADDR(0), PELI_RXB(6));
        p_can[0]->data[2] = READ_REG_BYTE(ADDR(0), PELI_RXB(7));
        p_can[0]->data[3] = READ_REG_BYTE(ADDR(0), PELI_RXB(8));
        p_can[0]->data[4] = READ_REG_BYTE(ADDR(0), PELI_RXB(9));
        p_can[0]->data[5] = READ_REG_BYTE(ADDR(0), PELI_RXB(10));
        p_can[0]->data[6] = READ_REG_BYTE(ADDR(0), PELI_RXB(11));
        p_can[0]->data[7] = READ_REG_BYTE(ADDR(0), PELI_RXB(12));
        *(u32 *)p_can[0]->id >>= 3;
        if (p_can[0]->id[1] == CA_MAIN && sys_ecu[p_can[0]->id[0]].msg) {
                p_can[0]->ts = tickGet();
                send[1] = (int)p_can[0];
                msgQSend(sys_ecu[p_can[0]->id[0]].msg, (str)send, 8, NO_WAIT, MSG_PRI_NORMAL);
                p_can[0] = (CAN *)lstNext((NODE *)p_can[0]);
        }
        WRITE_REG_BYTE(ADDR(0), PELI_CMR, 0x04);
}

static void isr_rx_can1(void)
{
        int send[2] = {tid_can, 0};
        if (READ_REG_BYTE(ADDR(1), PELI_IR) != 0x01 ||
            (READ_REG_BYTE(ADDR(1), PELI_SR) & 0x03) != 0x01 ||
            READ_REG_BYTE(ADDR(1), PELI_RXB(0)) != CAN_FF) {
                WRITE_REG_BYTE(ADDR(1), PELI_CMR, 0x04);
                return;
        }
        p_can[1]->id[3] = READ_REG_BYTE(ADDR(1), PELI_RXB(1));
        p_can[1]->id[2] = READ_REG_BYTE(ADDR(1), PELI_RXB(2));
        p_can[1]->id[1] = READ_REG_BYTE(ADDR(1), PELI_RXB(3));
        p_can[1]->id[0] = READ_REG_BYTE(ADDR(1), PELI_RXB(4));
        p_can[1]->data[0] = READ_REG_BYTE(ADDR(1), PELI_RXB(5));
        p_can[1]->data[1] = READ_REG_BYTE(ADDR(1), PELI_RXB(6));
        p_can[1]->data[2] = READ_REG_BYTE(ADDR(1), PELI_RXB(7));
        p_can[1]->data[3] = READ_REG_BYTE(ADDR(1), PELI_RXB(8));
        p_can[1]->data[4] = READ_REG_BYTE(ADDR(1), PELI_RXB(9));
        p_can[1]->data[5] = READ_REG_BYTE(ADDR(1), PELI_RXB(10));
        p_can[1]->data[6] = READ_REG_BYTE(ADDR(1), PELI_RXB(11));
        p_can[1]->data[7] = READ_REG_BYTE(ADDR(1), PELI_RXB(12));
        *(u32 *)p_can[1]->id >>= 3;
        if (p_can[1]->id[1] == CA_MAIN && sys_ecu[p_can[1]->id[0]].msg) {
                p_can[1]->ts = tickGet();
                send[1] = (int)p_can[1];
                msgQSend(sys_ecu[p_can[1]->id[0]].msg, (str)send, 8, NO_WAIT, MSG_PRI_NORMAL);
                p_can[1] = (CAN *)lstNext((NODE *)p_can[1]);
        }
        WRITE_REG_BYTE(ADDR(1), PELI_CMR, 0x04);
}

static void init_can0(void)
{
        sysIntDisablePIC(5);
        WRITE_REG_BYTE(ADDR(0), PELI_MODE, 0x09);
        WRITE_REG_BYTE(ADDR(0), PELI_CMR, 0x0C);
        WRITE_REG_BYTE(ADDR(0), PELI_CDR, 0x88);
        WRITE_REG_BYTE(ADDR(0), PELI_IER, 0x09);
        WRITE_REG_BYTE(ADDR(0), PELI_ACR(0), 0xFF);
        WRITE_REG_BYTE(ADDR(0), PELI_ACR(1), 0xFF);
        WRITE_REG_BYTE(ADDR(0), PELI_ACR(2), 0xFF);
        WRITE_REG_BYTE(ADDR(0), PELI_ACR(3), 0xFF);
        WRITE_REG_BYTE(ADDR(0), PELI_AMR(0), 0xFF);
        WRITE_REG_BYTE(ADDR(0), PELI_AMR(1), 0xFF);
        WRITE_REG_BYTE(ADDR(0), PELI_AMR(2), 0xFF);
        WRITE_REG_BYTE(ADDR(0), PELI_AMR(3), 0xFF);
        WRITE_REG_BYTE(ADDR(0), PELI_BTR(0), 0x04);
        WRITE_REG_BYTE(ADDR(0), PELI_BTR(1), 0x1C);
        WRITE_REG_BYTE(ADDR(0), PELI_EWLR, 0x60);
        WRITE_REG_BYTE(ADDR(0), PELI_OCR, 0x1A);
        WRITE_REG_BYTE(ADDR(0), PELI_MODE, 0x08);
        intConnect(INUM_TO_IVEC(sysInumTbl[5]), (VOIDFUNCPTR)isr_rx_can0, 0);
        sysIntEnablePIC(5);
}

static void init_can1(void)
{
        sysIntDisablePIC(7);
        WRITE_REG_BYTE(ADDR(1), PELI_MODE, 0x09);
        WRITE_REG_BYTE(ADDR(1), PELI_CMR, 0x0C);
        WRITE_REG_BYTE(ADDR(1), PELI_CDR, 0x88);
        WRITE_REG_BYTE(ADDR(1), PELI_IER, 0x09);
        WRITE_REG_BYTE(ADDR(1), PELI_ACR(0), 0xFF);
        WRITE_REG_BYTE(ADDR(1), PELI_ACR(1), 0xFF);
        WRITE_REG_BYTE(ADDR(1), PELI_ACR(2), 0xFF);
        WRITE_REG_BYTE(ADDR(1), PELI_ACR(3), 0xFF);
        WRITE_REG_BYTE(ADDR(1), PELI_AMR(0), 0xFF);
        WRITE_REG_BYTE(ADDR(1), PELI_AMR(1), 0xFF);
        WRITE_REG_BYTE(ADDR(1), PELI_AMR(2), 0xFF);
        WRITE_REG_BYTE(ADDR(1), PELI_AMR(3), 0xFF);
        WRITE_REG_BYTE(ADDR(1), PELI_BTR(0), 0x04);
        WRITE_REG_BYTE(ADDR(1), PELI_BTR(1), 0x1C);
        WRITE_REG_BYTE(ADDR(1), PELI_EWLR, 0x60);
        WRITE_REG_BYTE(ADDR(1), PELI_OCR, 0x1A);
        WRITE_REG_BYTE(ADDR(1), PELI_MODE, 0x08);
        intConnect(INUM_TO_IVEC(sysInumTbl[7]), (VOIDFUNCPTR)isr_rx_can1, 0);
        sysIntEnablePIC(7);
}

/*
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

  +------------+
  | CAN  | IRQ |
  +------+-----+
  | CAN0 | 5   |
  | CAN1 | 7   |
  | CAN2 | 11  |
  | CAN3 | 12  |
  +------+-----+
*/
