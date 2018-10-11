#ifndef CMD_H_
#define CMD_H_

#include "vx.h"

#define HAMMING_74(x)                              \
((x) & 8 ^ ((x) & 4) << 1 ^ ((x) & 1) << 3 |       \
((x) & 8) >> 1 ^ ((x) & 2) << 1 ^ ((x) & 1) << 2 | \
((x) & 4) >> 1 ^ (x) & 2 ^ ((x) & 1) << 1 |        \
(x) << 4)

struct cmd {
        NODE node;
        unsigned short head; /* 固定帧头 */
        unsigned short len; /* 从head开始到chk结束共计字节总数 */
        unsigned char src;
        unsigned char dev;
        unsigned char mode;
        unsigned char act;
        union {
                union {
                        struct {
                                unsigned char link: 2; /* 通信 */
                                unsigned char pmin: 2; /* 俯仰过小 */
                                unsigned char pmax: 2; /*  */
                                unsigned char rmin: 2; /*  */
                                unsigned char rmax: 2; /* 横滚过大 */
                                unsigned char dp: 2; /* 俯仰突变 */
                                unsigned char dr: 2; /*  */
                                unsigned char pdiff: 2; /* 俯仰不一致 */
                                unsigned char rdiff: 2; /*  */
                                unsigned char : 6;
                                unsigned char res;
                                unsigned pick; /* 选择 */
                        } relax; /* 降级使用 0/1/2/3：不降级/一次降级/二次降级/三次降级 */
                } tls; /* 倾角传感器 */
                union {
                        struct {
                                unsigned char link: 2; /*  */
                                unsigned char xmin: 2; /*  */
                                unsigned char xmax: 2; /*  */
                                unsigned char ymin: 2; /*  */
                                unsigned char ymax: 2; /*  */
                                unsigned char zmin: 2; /*  */
                                unsigned char zmax: 2; /*  */
                                unsigned char dmin: 2; /* 前后距离过小 */
                                unsigned char dmax: 2; /*  */
                                unsigned char : 6;
                                unsigned char res;
                                unsigned pick; /*  */
                        } relax;
                        struct {
                                unsigned char proc; /*  */
                                unsigned char res[3];
                                unsigned pick; /*  */
                        } photo;
                } vsl;
                union {
                        struct {
                                unsigned char link: 2; /*  */
                                unsigned char v24min: 2; /* 24V电压过低 */
                                unsigned char v24max: 2; /*  */
                                unsigned char a24min: 2; /*  */
                                unsigned char a24max: 2; /*  */
                                unsigned char v500min: 2; /*  */
                                unsigned char v500max: 2; /*  */
                                unsigned char a500min: 2; /*  */
                                unsigned char a500max: 2; /*  */
                                unsigned char dv24: 2; /*  */
                                unsigned char da24: 2; /*  */
                                unsigned char dv500: 2; /*  */
                                unsigned char da500: 2; /* 500V电流突变 */
                                unsigned char : 6;
                                unsigned char res[4];
                        } relax;
                        struct {
                                unsigned char : 8;
                                unsigned char : 1;
                                unsigned char light: 1;
                                unsigned char m5: 1;
                                unsigned char m7: 1;
                                unsigned char : 4;
                                unsigned char mom: 1;
                                unsigned char shdb: 1;
                                unsigned char shdf: 1;
                                unsigned char : 5;
                                unsigned char shdst: 1;
                                unsigned char leg0: 1;
                                unsigned char leg3: 1;
                                unsigned char leg1: 1;
                                unsigned char leg2: 1;
                                unsigned char xyzb: 1;
                                unsigned char xyzf: 1;
                                unsigned char : 1;
                                unsigned char res[4];
                        } toggle;
                } psu;
                union {
                        struct {
                                unsigned char link: 2;
                                unsigned char sync: 2;
                                unsigned char pmin: 2; /* 位置反向超限 */
                                unsigned char pmax: 2;
                                unsigned char vmin: 2;
                                unsigned char vmax: 2; /* 速度正向超限 */
                                unsigned char amin: 2; /* 电流反向超限 */
                                unsigned char amax: 2;
                                unsigned char dp: 2;
                                unsigned char dv: 2;
                                unsigned char da: 2;
                                unsigned char smin: 2; /* 行程反向超限 */
                                unsigned char smax: 2;
                                unsigned char : 6;
                                unsigned pick;
                        } relax;
                        struct {
                                unsigned char res[4];
                                unsigned pick;
                        } stupid; /* 自动模式 */
                        struct {
                                short vel;
                                unsigned char res[2];
                                unsigned pick;
                        } manual; /* 手动模式 */
                        struct {
                                short vel;
                                unsigned char res[2];
                                unsigned pick;
                        } expert; /* 专家模式 */
                } srv;
        } data;
        unsigned ts;
        unsigned char res[3];
        unsigned char chk;
};

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
