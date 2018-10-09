#ifndef CMD_H_
#define CMD_H_

#include "type.h"
#include "vx.h"

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
                union {
                        struct {
                                u8 link: 2;
                                u8 x: 2;
                                u8 y: 2;
                                u8 dx: 2;
                                u8 dy: 2;
                                u8 xd: 2;
                                u8 yd: 2;
                                u8 : 2;
                                u8 res[2];
                                u32 pick;
                        } relax;
                } tls;
                union {
                        struct {
                                u8 link: 2;
                                u8 xmin: 2;
                                u8 xmax: 2;
                                u8 ymin: 2;
                                u8 ymax: 2;
                                u8 zmin: 2;
                                u8 zmax: 2;
                                u8 toonear: 2;
                                u8 toofar: 2;
                                u8 : 6;
                                u8 res;
                                u32 pick;
                        } relax;
                        struct {
                                u8 proc;
                                u8 res[3];
                                u32 pick;
                        } photo;
                } vsl;
                union {
                        struct {
                                u8 link: 2;
                                u8 v24min: 2;
                                u8 v24max: 2;
                                u8 a24min: 2;
                                u8 a24max: 2;
                                u8 v500min: 2;
                                u8 v500max: 2;
                                u8 a500min: 2;
                                u8 a500max: 2;
                                u8 dv24: 2;
                                u8 da24: 2;
                                u8 dv500: 2;
                                u8 da500: 2;
                                u8 : 6;
                                u8 res[4];
                        } relax;
                        struct {
                                u8 : 8;
                                u8 : 1;
                                u8 light: 1;
                                u8 m5: 1;
                                u8 m7: 1;
                                u8 : 4;
                                u8 mom: 1;
                                u8 shdb: 1;
                                u8 shdf: 1;
                                u8 : 5;
                                u8 shdst: 1;
                                u8 leg0: 1;
                                u8 leg3: 1;
                                u8 leg1: 1;
                                u8 leg2: 1;
                                u8 xyzb: 1;
                                u8 xyzf: 1;
                                u8 : 1;
                                u8 res[4];
                        } toggle;
                } psu;
                union {
                        struct {
                                u8 link: 2;
                                u8 sync: 2;
                                u8 pmin: 2;
                                u8 pmax: 2;
                                u8 vel: 2;
                                u8 amin: 2;
                                u8 amax: 2;
                                u8 dp: 2;
                                u8 dv: 2;
                                u8 da: 2;
                                u8 smin: 2;
                                u8 smax: 2;
                                u8 res;
                                u32 pick;
                        } relax;
                        struct {
                                u8 res[4];
                                u32 pick;
                        } stupid;
                        struct {
                                s16 vel;
                                u8 res[2];
                                u32 pick;
                        } manual;
                        struct {
                                s16 vel;
                                u8 res[2];
                                u32 pick;
                        } expert;
                } srv;
        } data;
        u32 ts;
        u8 res[3];
        u8 chk;
} CMD;

#define CMD_IDLE 0x00

#define CMD_SRC_UDP  0xee
#define CMD_SRC_CORE 0xcc

#define CMD_DEV_TLS HAMMING_74(0x1)
#define CMD_DEV_VSL HAMMING_74(0x2)
#define CMD_DEV_PSU HAMMING_74(0x3)
#define CMD_DEV_SWH HAMMING_74(0x4)
#define CMD_DEV_RSE HAMMING_74(0x5)
#define CMD_DEV_SWV HAMMING_74(0x6)
#define CMD_DEV_PRP HAMMING_74(0x7)
#define CMD_DEV_XYZ HAMMING_74(0x8)
#define CMD_DEV_SHD HAMMING_74(0x9)
#define CMD_DEV_MOM HAMMING_74(0xa)
#define CMD_DEV_GEN HAMMING_74(0xb)
#define CMD_SRV_ALL HAMMING_74(0xc)

#define CMD_MODE_RELAX  HAMMING_74(0x1)
#define CMD_MODE_STUPID HAMMING_74(0x2)
#define CMD_MODE_MANUAL HAMMING_74(0x3)
#define CMD_MODE_EXPERT HAMMING_74(0x4)
#define CMD_MODE_PASSWD HAMMING_74(0x5)
#define CMD_MODE_ZERO   HAMMING_74(0x6)
#define CMD_MODE_SAVE   HAMMING_74(0x7)

#define CMD_ACT_VSL_GET   HAMMING_74(0x1)
#define CMD_ACT_VSL_PUT   HAMMING_74(0x2)
#define CMD_ACT_VSL_JOINT HAMMING_74(0x3)
#define CMD_ACT_PSU_OFF   HAMMING_74(0x4)
#define CMD_ACT_PSU_ON    HAMMING_74(0x5)
#define CMD_ACT_SRV_MIN   HAMMING_74(0x6)
#define CMD_ACT_SRV_MAX   HAMMING_74(0x7)
#define CMD_ACT_SRV_MIDA  HAMMING_74(0x8)
#define CMD_ACT_SRV_MIDB  HAMMING_74(0x9)
#define CMD_ACT_SRV_STOP  HAMMING_74(0xa)
#define CMD_ACT_SRV_POSI  HAMMING_74(0xb)
#define CMD_ACT_SRV_NEGA  HAMMING_74(0xc)

#endif /* CMD_H_ */
