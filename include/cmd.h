#ifndef CMD_H_
#define CMD_H_

#define HAMMING_74(x)                              \
((x) & 8 ^ ((x) & 4) << 1 ^ ((x) & 1) << 3 |       \
((x) & 8) >> 1 ^ ((x) & 2) << 1 ^ ((x) & 1) << 2 | \
((x) & 4) >> 1 ^ (x) & 2 ^ ((x) & 1) << 1 |        \
(x) << 4)

typedef struct {
        NODE node;
        u16 head;
        u16 len;
        u8 src;
        u8 dev;
        u8 mode;
        u8 act;
        union {
                str data;
                struct {
                        u8 relx: 2;
                        u8 : 6;
                        u8 res;
                        u16 valid;
                } tls;
                struct {
                        u8 relx_x0: 2;
                        u8 relx_x1: 2;
                        u8 relx_y01: 2;
                        u8 relx_y23: 2;
                        u8 relx_z0: 2;
                        u8 relx_z1: 2;
                        u8 res: 4;
                        u16 valid;
                } vsl;
                struct {
                        u32 : 8;
                        u32 : 1;
                        u32 light: 1;
                        u32 m5: 1;
                        u32 m7: 1;
                        u32 : 4;
                        u32 mom: 1;
                        u32 shdb: 1;
                        u32 shdf: 1;
                        u32 : 5;
                        u32 shdts: 1;
                        u32 leg0: 1;
                        u32 leg3: 1;
                        u32 leg1: 1;
                        u32 leg2: 1;
                        u32 xyzb: 1;
                        u32 xyzf: 1;
                        u32 : 1;
                } psu;
                struct {
                        u8 relx_ampr: 2;
                        u8 relx_sync: 2;
                        u8 res: 4;
                        u8 manu_vel: 4;
                        u8 expt_vel: 4;
                        u16 valid;
                } srv;
        } data;
        u32 ts;
        u8 res[3];
        u8 chk;
} CMD;

#define CMD_IDLE 0x00

#define CMD_SRC_UDP  0xec
#define CMD_SRC_CORE 0xcc

#define CMD_DEV_TLS  HAMMING_74(0x1)
#define CMD_DEV_VSL  HAMMING_74(0x2)
#define CMD_DEV_PSU  HAMMING_74(0x3)
#define CMD_DEV_SWH  HAMMING_74(0x4)
#define CMD_DEV_RSE  HAMMING_74(0x5)
#define CMD_DEV_SWV  HAMMING_74(0x6)
#define CMD_DEV_PRP  HAMMING_74(0x7)
#define CMD_DEV_XYZ  HAMMING_74(0x8)
#define CMD_DEV_SHD  HAMMING_74(0x9)
#define CMD_DEV_MOM  HAMMING_74(0xa)
#define CMD_DEV_GEN  HAMMING_74(0xb)
#define CMD_SRV_ALL  HAMMING_74(0xc)

#define CMD_MODE_STUPID HAMMING_74(0x1)
#define CMD_MODE_MANUAL HAMMING_74(0x2)
#define CMD_MODE_EXPERT HAMMING_74(0x3)
#define CMD_MODE_PASSWD HAMMING_74(0x4)
#define CMD_MODE_ZERO   HAMMING_74(0x5)
#define CMD_MODE_SAVE   HAMMING_74(0x6)

#define CMD_ACT_RELAX     HAMMING_74(0x1)
#define CMD_ACT_VSL_GET   HAMMING_74(0x2)
#define CMD_ACT_VSL_PUT   HAMMING_74(0x3)
#define CMD_ACT_VSL_JOINT HAMMING_74(0x4)
#define CMD_ACT_PSU_OFF   HAMMING_74(0x5)
#define CMD_ACT_PSU_ON    HAMMING_74(0x6)
#define CMD_ACT_SRV_MIN   HAMMING_74(0x7)
#define CMD_ACT_SRV_MAX   HAMMING_74(0x8)
#define CMD_ACT_SRV_MIDA  HAMMING_74(0x9)
#define CMD_ACT_SRV_MIDB  HAMMING_74(0xa)
#define CMD_ACT_SRV_STOP  HAMMING_74(0xb)
#define CMD_ACT_SRV_POSI  HAMMING_74(0xc)
#define CMD_ACT_SRV_NEGA  HAMMING_74(0xd)

#endif /* CMD_H_ */
