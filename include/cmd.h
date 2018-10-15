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
        unsigned short head; /* 固定帧头：0xecec */
        unsigned short len; /* 从head开始到chk结束共计字节总数 */
        unsigned char src; /* 数据源：0xec */
        unsigned char dev; /* 操作对象 */
        unsigned char mode; /* 模式 */
        unsigned char act; /* 动作 */
        union {
                union {
                        struct {
                                unsigned char link: 2; /* 通信 */
                                unsigned char pmin: 2; /* 俯仰过小 */
                                unsigned char pmax: 2; /* 俯仰过大 */
                                unsigned char rmin: 2; /* 横滚过小 */
                                unsigned char rmax: 2; /* 横滚过大 */
                                unsigned char dp: 2; /* 俯仰突变 */
                                unsigned char dr: 2; /* 横滚突变 */
                                unsigned char pdiff: 2; /* 俯仰不一致 */
                                unsigned char rdiff: 2; /* 横滚不一致 */
                                unsigned char : 6;
                                unsigned char res;
                                unsigned pick; /* 选择 */
                        } relax; /* 降级使用 0/1/2/3：不降级/一次降级/二次降级/三次降级 */
                } tls; /* 倾角传感器 */
                union {
                        struct {
                                unsigned char link: 2; /* 通信 */
                                unsigned char xmin: 2; /* X轴向测量值过小 */
                                unsigned char xmax: 2; /* X轴向测量值过大 */
                                unsigned char ymin: 2; /* Y轴向测量值过小 */
                                unsigned char ymax: 2; /* Y轴向测量值过大 */
                                unsigned char zmin: 2; /* Z轴向测量值过小 */
                                unsigned char zmax: 2; /* Z轴向测量值过大 */
                                unsigned char dmin: 2; /* 前后距离过小 */
                                unsigned char dmax: 2; /* 前后距离过大 */
                                unsigned char : 6;
                                unsigned char res;
                                unsigned pick; /* 选择 */
                        } relax; /* 降级使用 0/1/2/3：不降级/一次降级/二次降级/三次降级 */
                        struct {
                                unsigned char proc; /* 阶段 */
                                unsigned char res[3];
                                unsigned pick; /* 选择 */
                        } photo; /* 拍照 */
                } vsl; /* 视觉定位 */
                union {
                        struct {
                                unsigned char link: 2; /* 通信 */
                                unsigned char v24min: 2; /* 24V电压过低 */
                                unsigned char v24max: 2; /* 24V电压过高 */
                                unsigned char a24min: 2; /* 24V电流过低 */
                                unsigned char a24max: 2; /* 24V电流过高 */
                                unsigned char v500min: 2; /* 500V电压过低 */
                                unsigned char v500max: 2; /* 500V电压过高 */
                                unsigned char a500min: 2; /* 500V电流过低 */
                                unsigned char a500max: 2; /* 500V电流过高 */
                                unsigned char dv24: 2; /* 24V电压突变 */
                                unsigned char da24: 2; /* 24V电流突变 */
                                unsigned char dv500: 2; /* 500V电流突变 */
                                unsigned char da500: 2; /* 500V电流突变 */
                                unsigned char : 6;
                                unsigned char res[4];
                        } relax; /* 降级使用 0/1/2/3：不降级/一次降级/二次降级/三次降级 */
                        struct {
                                unsigned char : 8;
                                unsigned char : 1;
                                unsigned char light: 1; /* 照明灯 */
                                unsigned char m5: 1; /* 5m定位灯 */
                                unsigned char m7: 1; /* 7m定位灯 */
                                unsigned char : 4;
                                unsigned char mom: 1; /* 恒力矩 */
                                unsigned char shdb: 1; /* 后端帘 */
                                unsigned char shdf: 1; /* 前端帘 */
                                unsigned char : 5;
                                unsigned char shdst: 1; /* 侧帘/纵展 */
                                unsigned char leg0: 1; /* 左前支腿 */
                                unsigned char leg3: 1; /* 右前支腿 */
                                unsigned char leg1: 1; /* 左后支腿 */
                                unsigned char leg2: 1; /* 右后支腿 */
                                unsigned char xyzb: 1; /* 后三轴转载机构 */
                                unsigned char xyzf: 1; /* 前三轴转载机构 */
                                unsigned char : 1;
                                unsigned char res[4];
                        } v24; /* 24V开关 */
                        struct {
                                unsigned char : 8;
                                unsigned char : 8;
                                unsigned char mom: 1; /* 恒力矩 */
                                unsigned char shdb: 1; /* 后端帘 */
                                unsigned char shdf: 1; /* 前端帘 */
                                unsigned char : 5;
                                unsigned char shdst: 1; /* 侧帘/纵展 */
                                unsigned char leg0: 1; /* 左前支腿 */
                                unsigned char leg3: 1; /* 右前支腿 */
                                unsigned char leg1: 1; /* 左后支腿 */
                                unsigned char leg2: 1; /* 右后支腿 */
                                unsigned char xyzb: 1; /* 后三轴转载机构 */
                                unsigned char xyzf: 1; /* 前三轴转载机构 */
                                unsigned char : 1;
                                unsigned char res[4];
                        } v500; /* 500V开关 */
                } psu; /* 供电单元 */
                union {
                        struct {
                                unsigned char link: 2; /* 通信中断 */
                                unsigned char sync: 2; /* 同步异常 */
                                unsigned char pmin: 2; /* 位置反向超限 */
                                unsigned char pmax: 2; /* 位置正向超限 */
                                unsigned char vmin: 2; /* 速度反向超限 */
                                unsigned char vmax: 2; /* 速度正向超限 */
                                unsigned char amin: 2; /* 电流反向超限 */
                                unsigned char amax: 2; /* 电流正向超限 */
                                unsigned char dp: 2; /* 位置突变 */
                                unsigned char dv: 2; /* 速度突变 */
                                unsigned char da: 2; /* 电流突变 */
                                unsigned char smin: 2; /* 行程反向超限 */
                                unsigned char smax: 2; /* 行程正向超限 */
                                unsigned char : 6;
                                unsigned pick; /* 选择 */
                        } relax; /* 降级使用 0/1/2/3：不降级/一次降级/二次降级/三次降级 */
                        struct {
                                unsigned char res[4];
                                unsigned pick; /* 选择 */
                        } stupid; /* 自动模式 */
                        struct {
                                short vel; /* 速度 单位：0.01mm/s */
                                unsigned char res[2];
                                unsigned pick; /* 选择 */
                        } manual; /* 手动模式 */
                        struct {
                                short vel; /* 速度 单位：0.01mm/s */
                                unsigned char res[2];
                                unsigned pick; /* 选择 */
                        } expert; /* 专家模式 */
                } srv; /* 伺服驱动器 */
        } data; /* 数据 */
        unsigned ts; /* 时标 单位：10ms */
        unsigned char res[3];
        unsigned char chk; /* 校验 */
};

#define CMD_IDLE 0x00 /* 空闲 */

#define CMD_DEV_TLS HAMMING_74(0x1) /* 0x1e：倾角传感器 */
#define CMD_DEV_VSL HAMMING_74(0x2) /* 0x26：视觉定位 */
#define CMD_DEV_PSU HAMMING_74(0x3) /* 0x38：供电单元 */
#define CMD_DEV_SWH HAMMING_74(0x4) /* 0x4a：横展 */
#define CMD_DEV_RSE HAMMING_74(0x5) /* 0x54：顶升 */
#define CMD_DEV_SWV HAMMING_74(0x6) /* 0x6c：俯仰 */
#define CMD_DEV_PRP HAMMING_74(0x7) /* 0x72：垂直 */
#define CMD_DEV_XYZ HAMMING_74(0x8) /* 0x8c：三轴转载机构 */
#define CMD_DEV_SHD HAMMING_74(0x9) /* 0x92：防护棚 */
#define CMD_DEV_MOM HAMMING_74(0xa) /* 0xaa：恒力矩 */
#define CMD_DEV_GEN HAMMING_74(0xb) /* 0xb4：发电机 */
#define CMD_SRV_ALL HAMMING_74(0xc) /* 0xc6：所有伺服 */

#define CMD_MODE_RELAX      HAMMING_74(0x1) /* 0x1e：降级使用 */
#define CMD_MODE_VSL_PHOTO  HAMMING_74(0x2) /* 0x26：拍照 */
#define CMD_MODE_PSU_V24    HAMMING_74(0x3) /* 0x38：24V开关 */
#define CMD_MODE_PSU_V500   HAMMING_74(0x4) /* 0x4a：500V开关 */
#define CMD_MODE_SRV_STUPID HAMMING_74(0x5) /* 0x54：自动模式 */
#define CMD_MODE_SRV_MANUAL HAMMING_74(0x6) /* 0x6c：手动模式 */
#define CMD_MODE_SRV_EXPERT HAMMING_74(0x7) /* 0x72：专家模式 */
#define CMD_MODE_SRV_PASSWD HAMMING_74(0x8) /* 0x8c：密码 */
#define CMD_MODE_SRV_ZERO   HAMMING_74(0x9) /* 0x92：标零 */
#define CMD_MODE_SRV_SAVE   HAMMING_74(0xa) /* 0xaa：保存 */

#define CMD_ACT_VSL_GET   HAMMING_74(0x1) /* 0x1e：吊载 */
#define CMD_ACT_VSL_PUT   HAMMING_74(0x2) /* 0x26：转载 */
#define CMD_ACT_VSL_JOINT HAMMING_74(0x3) /* 0x38：撤收对接 */
#define CMD_ACT_PSU_OFF   HAMMING_74(0x4) /* 0x4a：断电 */
#define CMD_ACT_PSU_ON    HAMMING_74(0x5) /* 0x54：上电 */
#define CMD_ACT_SRV_MIN   HAMMING_74(0x6) /* 0x6c：最小位置 */
#define CMD_ACT_SRV_MAX   HAMMING_74(0x7) /* 0x72：最大位置 */
#define CMD_ACT_SRV_MIDA  HAMMING_74(0x8) /* 0x8c：中间位置A */
#define CMD_ACT_SRV_MIDB  HAMMING_74(0x9) /* 0x92：中间位置B */
#define CMD_ACT_SRV_STOP  HAMMING_74(0xa) /* 0xaa：停止 */
#define CMD_ACT_SRV_POSI  HAMMING_74(0xb) /* 0xb4：正转 */
#define CMD_ACT_SRV_NEGA  HAMMING_74(0xc) /* 0xc6：反转 */

#endif /* CMD_H_ */
