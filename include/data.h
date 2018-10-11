#ifndef DATA_H_
#define DATA_H_

struct data {
        unsigned short head; /* 固定帧头：0xCCCC */
        unsigned short len; /* 从head开始到chk结束共计字节总数 */
        struct {
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
                        unsigned char : 6;
                        unsigned char dev: 2; /* 硬件故障 */
                } fault; /*故障 0/1/2/3：正常/警告/一般故障/严重故障*/
                short p; /* 俯仰倾角 */
                short r; /* 横滚倾角 */
                unsigned ts; /* 时标 */
        } tls[2]; /* 倾角传感器 */
        struct {
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
                        unsigned char : 6;
                        unsigned char dev: 2; /* 硬件故障 */
                } fault; /* 故障 0/1/2/3：正常/警告/一般故障/严重故障 */
                unsigned char res;
                unsigned char proc; /* 阶段 */
                short x; /* X轴测量值 */
                short y; /* Y轴测量值 */
                short z; /* Z轴测量值 */
                unsigned ts; /* 时标 */
        } vsl[2]; /* 视觉定位 */
        struct {
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
                        unsigned char : 4;
                        unsigned char dev: 2; /* 硬件故障 */
                } fault; /* 故障 0/1/2/3：正常/警告/一般故障/严重故障 */
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
                        unsigned short volt; /* 电压值 */
                        unsigned short ampr; /* 电流值 */
                } v24; /* 24V */
                struct {
                        unsigned char : 8;
                        unsigned char : 8;
                        unsigned char mom: 1; /* 恒力矩 */
                        unsigned char shdb: 1; /* 后端帘 */
                        unsigned char shdf: 1; /* 前端帘 */
                        unsigned char : 5;
                        unsigned char shdst: 1;
                        char shdst: 1; /* 侧帘/纵展 */
                        unsigned char leg0: 1; /* 左前支腿 */
                        unsigned char leg3: 1; /* 右前支腿 */
                        unsigned char leg1: 1; /* 左后支腿 */
                        unsigned char leg2: 1; /* 右后支腿 */
                        unsigned char xyzb: 1; /* 后三轴转载机构 */
                        unsigned char xyzf: 1; /* 前三轴转载机构 */
                        unsigned char : 1;
                        unsigned short volt; /* 电压值 */
                        unsigned short ampr; /* 电流值 */
                } v500; /* 500V */
                unsigned ts; /* 时标 */
        } psu; /* 供电单元 */
        struct {
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
                        unsigned char brake: 2; /* 抱闸异常 */
                        unsigned char limit: 2; /* 限位异常 */
                        unsigned char dev: 2; /* 硬件故障 */
                } fault; /* 故障 0/1/2/3：正常/警告/一般故障/严重故障 */
                struct {
                        unsigned char brake: 1; /* 抱闸开关 */
                        unsigned char min: 1; /* 最小限位开关 */
                        unsigned char max: 1; /* 最大限位开关 */
                        unsigned char : 5;
                } io; /* I/O状态 */
                struct {
                        unsigned char min: 1; /* 位置最小 */
                        unsigned char max: 1; /* 位置最大 */
                        unsigned char mida: 1; /* 中间a位 */
                        unsigned char midb: 1; /* 中间b位 */
                        unsigned char stop: 1; /* 停止 */
                        unsigned char loadp: 1; /* 正向带载 */
                        unsigned char loadn: 1; /* 反向带载 */
                        unsigned char lock: 1; /* 定位锁 */
                } misc; /* 其他 */
                unsigned char res[3];
                unsigned char err; /* 驱动器故障代码 */
                short pos; /* 位置 */
                short vel; /* 速度 */
                short ampr; /* 电流 */
                unsigned ts; /* 时标 */
        } srv[41]; /* 伺服驱动器 */
        unsigned ts; /* 时标 */
        struct {
                unsigned char bus0: 1; /* 总线0负载率 */
                unsigned char bus1: 1; /* 总线1负载率 */
                unsigned char : 6;
        } fault; /* 故障 0/1：负载率正常/过高 */
        unsigned char res[2];
        unsigned char chk; /* 校验 */
};

#define NORMAL  0 /* 正常 */
#define WARNING 1 /* 警告 */
#define GENERAL 2 /* 一般故障 */
#define SERIOUS 3 /* 严重故障 */

#endif /* DATA_H_ */
