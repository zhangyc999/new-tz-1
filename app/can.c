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

static void isr0(void);
static void isr1(void);
static void init0(void);
static void init1(void);

ECU* id2ecu(u8 id);

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
        msg = id2ecu(buf.id[0])->msg;
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
        msg = id2ecu(buf.id[0])->msg;
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

ECU* id2ecu(u8 id)
{
        const static ECU ecu[255];
        ecu[CA_TLS0].index = 1;
        ecu[CA_TLS1].index = 2;
        ecu[CA_VSL0].index = 3;
        ecu[CA_VSL1].index = 4;
        ecu[CA_PSU].index = 5;
        ecu[CA_SWH0].index = 6;
        ecu[CA_SWH1].index = 7;
        ecu[CA_SWH2].index = 8;
        ecu[CA_SWH3].index = 9;
        ecu[CA_RSE0].index = 10;
        ecu[CA_RSE1].index = 11;
        ecu[CA_RSE2].index = 12;
        ecu[CA_RSE3].index = 13;
        ecu[CA_SWV0].index = 14;
        ecu[CA_SWV1].index = 15;
        ecu[CA_SWV2].index = 16;
        ecu[CA_SWV3].index = 17;
        ecu[CA_PRP0].index = 18;
        ecu[CA_PRP1].index = 19;
        ecu[CA_PRP2].index = 20;
        ecu[CA_PRP3].index = 21;
        ecu[CA_X0].index = 22;
        ecu[CA_X1].index = 23;
        ecu[CA_Y0].index = 24;
        ecu[CA_Y1].index = 25;
        ecu[CA_Y2].index = 26;
        ecu[CA_Y3].index = 27;
        ecu[CA_Z0].index = 28;
        ecu[CA_Z1].index = 29;
        ecu[CA_SDT].index = 30;
        ecu[CA_SDS0].index = 31;
        ecu[CA_SDS1].index = 32;
        ecu[CA_SDS2].index = 33;
        ecu[CA_SDS3].index = 34;
        ecu[CA_SDF0].index = 35;
        ecu[CA_SDF1].index = 36;
        ecu[CA_SDF2].index = 37;
        ecu[CA_SDF3].index = 38;
        ecu[CA_SDB0].index = 39;
        ecu[CA_SDB1].index = 40;
        ecu[CA_SDB2].index = 41;
        ecu[CA_SDB3].index = 42;
        ecu[CA_MOM0].index = 43;
        ecu[CA_MOM1].index = 44;
        ecu[CA_MOM2].index = 45;
        ecu[CA_MOM3].index = 46;
        ecu[CA_GEN0].index = 47;
        ecu[CA_GEN1].index = 48;
        ecu[CA_TLS0].msg = msg_tls;
        ecu[CA_TLS1].msg = msg_tls;
        ecu[CA_VSL0].msg = msg_vsl;
        ecu[CA_VSL1].msg = msg_vsl;
        ecu[CA_PSU].msg = msg_psu;
        ecu[CA_SWH0].msg = msg_swh;
        ecu[CA_SWH1].msg = msg_swh;
        ecu[CA_SWH2].msg = msg_swh;
        ecu[CA_SWH3].msg = msg_swh;
        ecu[CA_RSE0].msg = msg_rse;
        ecu[CA_RSE1].msg = msg_rse;
        ecu[CA_RSE2].msg = msg_rse;
        ecu[CA_RSE3].msg = msg_rse;
        ecu[CA_SWV0].msg = msg_swv;
        ecu[CA_SWV1].msg = msg_swv;
        ecu[CA_SWV2].msg = msg_swv;
        ecu[CA_SWV3].msg = msg_swv;
        ecu[CA_PRP0].msg = msg_prp;
        ecu[CA_PRP1].msg = msg_prp;
        ecu[CA_PRP2].msg = msg_prp;
        ecu[CA_PRP3].msg = msg_prp;
        ecu[CA_X0].msg = msg_xyz;
        ecu[CA_X1].msg = msg_xyz;
        ecu[CA_Y0].msg = msg_xyz;
        ecu[CA_Y1].msg = msg_xyz;
        ecu[CA_Y2].msg = msg_xyz;
        ecu[CA_Y3].msg = msg_xyz;
        ecu[CA_Z0].msg = msg_xyz;
        ecu[CA_Z1].msg = msg_xyz;
        ecu[CA_SDT].msg = msg_sdt;
        ecu[CA_SDS0].msg = msg_sdt;
        ecu[CA_SDS1].msg = msg_sds;
        ecu[CA_SDS2].msg = msg_sds;
        ecu[CA_SDS3].msg = msg_sds;
        ecu[CA_SDF0].msg = msg_sds;
        ecu[CA_SDF1].msg = msg_sdfb;
        ecu[CA_SDF2].msg = msg_sdfb;
        ecu[CA_SDF3].msg = msg_sdfb;
        ecu[CA_SDB0].msg = msg_sdfb;
        ecu[CA_SDB1].msg = msg_sdfb;
        ecu[CA_SDB2].msg = msg_sdfb;
        ecu[CA_SDB3].msg = msg_sdfb;
        ecu[CA_MOM0].msg = msg_mom;
        ecu[CA_MOM1].msg = msg_mom;
        ecu[CA_MOM2].msg = msg_mom;
        ecu[CA_MOM3].msg = msg_mom;
        ecu[CA_GEN0].msg = msg_gen;
        ecu[CA_GEN1].msg = msg_gen;
        return &ecu[id];
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
