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

extern MSG_Q_ID msg_can[2][3];
extern MSG_Q_ID msg_tls;
extern MSG_Q_ID msg_vsl;
extern MSG_Q_ID msg_psu;
extern MSG_Q_ID msg_swh;
extern MSG_Q_ID msg_rse;
extern MSG_Q_ID msg_swv;
extern MSG_Q_ID msg_prp;
extern MSG_Q_ID msg_xyz;
extern MSG_Q_ID msg_sdt;
extern MSG_Q_ID msg_sds;
extern MSG_Q_ID msg_sdfb;
extern MSG_Q_ID msg_mom;
extern MSG_Q_ID msg_gen;

const extern ECU sys_ecu[255];

static void isr0(void);
static void isr1(void);
static void init0(void);
static void init1(void);

void t_can(int period)
{
        static int i;
        static CAN buf;
        init0();
        init1();
        for (;;) {
                taskDelay(period);
                for (i = 0; i < 2; i++) {
                        if (READ_REG_BYTE(ADDR(i), PELI_SR) & 0x80) {
                                if (!i)
                                        init0();
                                else
                                        init1();
                                continue;
                        }
                        if (ERROR != msgQReceive(msg_can[i][0], (str)&buf, sizeof(buf), NO_WAIT) ||
                            ERROR != msgQReceive(msg_can[i][1], (str)&buf, sizeof(buf), NO_WAIT) ||
                            ERROR != msgQReceive(msg_can[i][2], (str)&buf, sizeof(buf), NO_WAIT)) {
                                *(u32 *)buf.id <<= 3;
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(0), CAN_FF);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(1), buf.id[3]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(2), buf.id[2]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(3), buf.id[1]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(4), buf.id[0]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(5), buf.data[0]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(6), buf.data[1]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(7), buf.data[2]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(8), buf.data[3]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(9), buf.data[4]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(10), buf.data[5]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(11), buf.data[6]);
                                WRITE_REG_BYTE(ADDR(i), PELI_TXB(12), buf.data[7]);
                                WRITE_REG_BYTE(ADDR(i), PELI_CMR, 0x01);
                        }
                }
        }
}

static void isr0(void)
{
        static CAN buf;
        static MSG_Q_ID msg;
        if (READ_REG_BYTE(ADDR(0), PELI_IR) != 0x01 ||
            (READ_REG_BYTE(ADDR(0), PELI_SR) & 0x03) != 0x01 ||
            READ_REG_BYTE(ADDR(0), PELI_RXB(0)) != CAN_FF) {
                WRITE_REG_BYTE(ADDR(0), PELI_CMR, 0x04);
                return;
        }
        buf.id[3] = READ_REG_BYTE(ADDR(0), PELI_RXB(1));
        buf.id[2] = READ_REG_BYTE(ADDR(0), PELI_RXB(2));
        buf.id[1] = READ_REG_BYTE(ADDR(0), PELI_RXB(3));
        buf.id[0] = READ_REG_BYTE(ADDR(0), PELI_RXB(4));
        *(u32 *)&buf.id[0] >>= 3;
        msg = sys_ecu(buf.id[0])->msg;
        if (!msg || buf.id[1] != CA_MAIN) {
                WRITE_REG_BYTE(ADDR(0), PELI_CMR, 0x04);
                return;
        }
        buf.data[0] = READ_REG_BYTE(ADDR(0), PELI_RXB(5));
        buf.data[1] = READ_REG_BYTE(ADDR(0), PELI_RXB(6));
        buf.data[2] = READ_REG_BYTE(ADDR(0), PELI_RXB(7));
        buf.data[3] = READ_REG_BYTE(ADDR(0), PELI_RXB(8));
        buf.data[4] = READ_REG_BYTE(ADDR(0), PELI_RXB(9));
        buf.data[5] = READ_REG_BYTE(ADDR(0), PELI_RXB(10));
        buf.data[6] = READ_REG_BYTE(ADDR(0), PELI_RXB(11));
        buf.data[7] = READ_REG_BYTE(ADDR(0), PELI_RXB(12));
        WRITE_REG_BYTE(ADDR(0), PELI_CMR, 0x04);
        buf.ts = tickGet();
        msgQSend(msg, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
}

static void isr1(void)
{
        static CAN buf;
        static MSG_Q_ID msg;
        if (READ_REG_BYTE(ADDR(1), PELI_IR) != 0x01 ||
            (READ_REG_BYTE(ADDR(1), PELI_SR) & 0x03) != 0x01 ||
            READ_REG_BYTE(ADDR(1), PELI_RXB(0)) != CAN_FF) {
                WRITE_REG_BYTE(ADDR(1), PELI_CMR, 0x04);
                return;
        }
        buf.id[3] = READ_REG_BYTE(ADDR(1), PELI_RXB(1));
        buf.id[2] = READ_REG_BYTE(ADDR(1), PELI_RXB(2));
        buf.id[1] = READ_REG_BYTE(ADDR(1), PELI_RXB(3));
        buf.id[0] = READ_REG_BYTE(ADDR(1), PELI_RXB(4));
        *(u32 *)&buf.id[0] >>= 3;
        msg = sys_ecu(buf.id[0])->msg;
        if (!msg || buf.id[1] != CA_MAIN) {
                WRITE_REG_BYTE(ADDR(1), PELI_CMR, 0x04);
                return;
        }
        buf.data[0] = READ_REG_BYTE(ADDR(1), PELI_RXB(5));
        buf.data[1] = READ_REG_BYTE(ADDR(1), PELI_RXB(6));
        buf.data[2] = READ_REG_BYTE(ADDR(1), PELI_RXB(7));
        buf.data[3] = READ_REG_BYTE(ADDR(1), PELI_RXB(8));
        buf.data[4] = READ_REG_BYTE(ADDR(1), PELI_RXB(9));
        buf.data[5] = READ_REG_BYTE(ADDR(1), PELI_RXB(10));
        buf.data[6] = READ_REG_BYTE(ADDR(1), PELI_RXB(11));
        buf.data[7] = READ_REG_BYTE(ADDR(1), PELI_RXB(12));
        WRITE_REG_BYTE(ADDR(1), PELI_CMR, 0x04);
        buf.ts = tickGet();
        msgQSend(msg, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
}

static void init0(void)
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
        intConnect(INUM_TO_IVEC(sysInumTbl[5]), (VOIDFUNCPTR)isr0, 0);
        sysIntEnablePIC(5);
}

static void init1(void)
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
        intConnect(INUM_TO_IVEC(sysInumTbl[7]), (VOIDFUNCPTR)isr1, 0);
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
