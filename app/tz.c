#include "can.h"
#include "canaddr.h"
#include "cmd.h"
#include "data.h"
#include "vx.h"

extern void t_core(int period);
extern void t_udpr(int sfd, int period, int duration);
extern void t_udpt(int sfd, int portclient, int addrgroup, int period);
extern void t_can(int addr, int irq, int n, int period, int duration);
extern void t_tls(int ca, int n, int period, int duration);
extern void t_vsl(int ca, int n, int period, int duration);
extern void t_psu(int ca, int n, int period, int duration);
extern void t_swh(int ca, int n, int period, int duration);
extern void t_rse(int ca, int n, int period, int duration);
extern void t_swv(int ca, int n, int period, int duration);
extern void t_prp(int ca, int n, int period, int duration);
extern void t_xyz(int ca, int n, int period, int duration);
extern void t_shd(int ca, int n, int period, int duration);
extern void t_mom(int ca, int n, int period, int duration);
extern void t_dbg(int period);

int tid_core; /* 主任务ID */
int tid_udpr; /* UDP接收任务ID */
int tid_udpt; /* UDP发送任务ID */
int tid_can; /* CAN通信任务ID */
int tid_tls; /* 倾角传感器任务ID */
int tid_vsl; /* 视觉定位任务ID */
int tid_psu; /* 供电单元任务ID */
int tid_swh; /* 横展任务ID */
int tid_rse; /* 顶升任务ID */
int tid_swv; /* 俯仰任务ID */
int tid_prp; /* 垂直任务ID */
int tid_xyz; /* 三轴转载机构任务ID */
int tid_shd; /* 防护棚任务ID */
int tid_mom; /* 恒力矩任务ID */
int tid_dbg; /* 调试任务ID */
MSG_Q_ID msg_core; /* 主任务消息队列ID */
MSG_Q_ID msg_can[8][3]; /* CAN通信发送消息队列ID */
MSG_Q_ID msg_tls; /* 倾角传感器消息队列ID */
MSG_Q_ID msg_vsl; /* 视觉定位消息队列ID */
MSG_Q_ID msg_psu; /* 供电单元消息队列ID */
MSG_Q_ID msg_swh; /* 横展消息队列ID */
MSG_Q_ID msg_rse; /* 顶升消息队列ID */
MSG_Q_ID msg_swv; /* 俯仰消息队列ID */
MSG_Q_ID msg_prp; /* 垂直消息队列ID */
MSG_Q_ID msg_xyz; /* 三轴转载机构消息队列ID */
MSG_Q_ID msg_shd; /* 防护棚消息队列ID */
MSG_Q_ID msg_mom; /* 恒力矩消息队列ID */
MSG_Q_ID msg_gen; /* 发电机消息队列ID */
unsigned ev_core_vsl = VXEV01; /* 视觉定位开机自检准备好 */
unsigned ev_core_swh = VXEV02; /* 横展开机自检准备好 */
unsigned ev_core_rse = VXEV03; /* 顶升开机自检准备好 */
unsigned ev_core_swv = VXEV04; /* 俯仰开机自检准备好 */
unsigned ev_core_prp = VXEV05; /* 垂直开机自检准备好 */
unsigned ev_core_xyz = VXEV06; /* 三轴转载机构开机自检准备好 */
unsigned ev_core_shd = VXEV07; /* 防护棚开机自检准备好 */
unsigned ev_core_mom = VXEV08; /* 恒力矩开机自检准备好 */
struct data sys_data; /* 系统当前状态信息 */
struct ecu sys_ecu[256]; /* 各CAN节点索引及消息队列（CAN通信任务->相应子任务）*/

static void ecu_init(void); /* 初始化各CAN节点索引及消息队列 */

void tz(void)
{
        int sfd = socket(AF_INET, SOCK_DGRAM, 0); /* 创建套接字 */
        int size = sizeof(struct sockaddr_in); /* sockaddr_in结构体长度 */
        struct sockaddr_in server; /* 服务器 */
        struct ip_mreq group; /* 组播 */
        /* u_long mode = 1; */
        int portserver = 4207; /* 服务器（主控）端口 */
        int portclient = 4201; /* 客户端（手持）端口 */
        char *addrserver = "192.168.100.130"; /* 服务器地址 */
        char *addrgroup = "234.1.1.9"; /* 组播地址 */
        int can[8] = {0xD1000, 0xD3000, 0xD5000, 0xD7000}; /* CAN总线基地址表 */
        int irq[8] = {5, 7, 11, 12}; /* CAN总线中断号表 */
        unsigned char tls[16] = {CA_TLS0, CA_TLS1}; /* 倾角传感器CAN地址表 */
        unsigned char vsl[16] = {CA_VSL0, CA_VSL1}; /* 视觉定位CAN地址表 */
        unsigned char psu[16] = {CA_PSU}; /* 供电单元CAN地址表 */
        unsigned char swh[16] = {CA_SWH0, CA_SWH1, CA_SWH2, CA_SWH3}; /* 横展CAN地址表 */
        unsigned char rse[16] = {CA_RSE0, CA_RSE1, CA_RSE2, CA_RSE3}; /* 顶升CAN地址表 */
        unsigned char swv[16] = {CA_SWV0, CA_SWV1, CA_SWV2, CA_SWV3}; /* 俯仰CAN地址表 */
        unsigned char prp[16] = {CA_PRP0, CA_PRP1, CA_PRP2, CA_PRP3}; /* 垂直CAN地址表 */
        unsigned char xyz[16] = {CA_X0, CA_X1, CA_Y0, CA_Y1, CA_Y2, CA_Y3, CA_Z0, CA_Z1}; /* 三轴转载机构CAN地址表 */
        unsigned char shd[16] = {
                CA_SHDF0, CA_SHDF1, CA_SHDF2, CA_SHDF3,
                CA_SHDB0, CA_SHDB1, CA_SHDB2, CA_SHDB3,
                CA_SHDS0, CA_SHDS1, CA_SHDS2, CA_SHDS3
        }; /* 防护棚CAN地址表 */
        unsigned char mom[16] = {CA_MOM0, CA_MOM1, CA_MOM2, CA_MOM3}; /* 恒力矩CAN地址表 */
        server.sin_len = (u_char)size;
        server.sin_family = AF_INET;
        server.sin_port = htons(portserver);
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        group.imr_interface.s_addr = inet_addr(addrserver);
        group.imr_multiaddr.s_addr = inet_addr(addrgroup);
        routeAdd(addrgroup, addrserver);
        /* ioctl(sfd, FIONBIO, (int)&mode); */
        bind(sfd, (struct sockaddr *)&server, size);
        setsockopt(sfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group));
        msg_core = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_can[0][0] = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_can[0][1] = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_can[0][2] = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_can[1][0] = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_can[1][1] = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_can[1][2] = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_tls = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_vsl = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_psu = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_swh = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_rse = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_swv = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_prp = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_xyz = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_shd = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_mom = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_gen = msgQCreate(128, 8, MSG_Q_FIFO);
        lstLibInit(); /* 双向链表功能初始化 */
        ecu_init(); /* 初始化各CAN节点索引及消息队列 */
        sysClkRateSet(100); /* 设置系统调用时间片 */
        tid_core = taskSpawn("CORE", 99, VX_FP_TASK, 20000, (FUNCPTR)t_core, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_udpr = taskSpawn("UDPR", 90, VX_FP_TASK, 200000, (FUNCPTR)t_udpr, sfd, 10, 100, 0, 0, 0, 0, 0, 0, 0);
        tid_udpt = taskSpawn("UDPT", 90, VX_FP_TASK, 20000, (FUNCPTR)t_udpt, sfd, portclient, (int)addrgroup, 10, 0, 0, 0, 0, 0, 0);
        tid_can = taskSpawn("CAN", 40, VX_FP_TASK, 200000, (FUNCPTR)t_can, (int)can, (int)irq, 2, 1, 1000, 0, 0, 0, 0, 0);
        tid_tls = taskSpawn("TLS", 50, VX_FP_TASK, 20000, (FUNCPTR)t_tls, (int)tls, 2, 50, 10, 0, 0, 0, 0, 0, 0);
}

static void ecu_init(void)
{
        sys_ecu[CA_TLS0].index = 1;
        sys_ecu[CA_TLS1].index = 2;
        sys_ecu[CA_VSL0].index = 3;
        sys_ecu[CA_VSL1].index = 4;
        sys_ecu[CA_PSU].index = 5;
        sys_ecu[CA_SWH0].index = 6;
        sys_ecu[CA_SWH1].index = 7;
        sys_ecu[CA_SWH2].index = 8;
        sys_ecu[CA_SWH3].index = 9;
        sys_ecu[CA_RSE0].index = 10;
        sys_ecu[CA_RSE1].index = 11;
        sys_ecu[CA_RSE2].index = 12;
        sys_ecu[CA_RSE3].index = 13;
        sys_ecu[CA_SWV0].index = 14;
        sys_ecu[CA_SWV1].index = 15;
        sys_ecu[CA_SWV2].index = 16;
        sys_ecu[CA_SWV3].index = 17;
        sys_ecu[CA_PRP0].index = 18;
        sys_ecu[CA_PRP1].index = 19;
        sys_ecu[CA_PRP2].index = 20;
        sys_ecu[CA_PRP3].index = 21;
        sys_ecu[CA_X0].index = 22;
        sys_ecu[CA_X1].index = 23;
        sys_ecu[CA_Y0].index = 24;
        sys_ecu[CA_Y1].index = 25;
        sys_ecu[CA_Y2].index = 26;
        sys_ecu[CA_Y3].index = 27;
        sys_ecu[CA_Z0].index = 28;
        sys_ecu[CA_Z1].index = 29;
        sys_ecu[CA_SHDF0].index = 30;
        sys_ecu[CA_SHDF1].index = 31;
        sys_ecu[CA_SHDF2].index = 32;
        sys_ecu[CA_SHDF3].index = 33;
        sys_ecu[CA_SHDB0].index = 34;
        sys_ecu[CA_SHDB1].index = 35;
        sys_ecu[CA_SHDB2].index = 36;
        sys_ecu[CA_SHDB3].index = 37;
        sys_ecu[CA_SHDS0].index = 38;
        sys_ecu[CA_SHDS1].index = 39;
        sys_ecu[CA_SHDS2].index = 40;
        sys_ecu[CA_SHDS3].index = 41;
        sys_ecu[CA_SHDT].index = 42;
        sys_ecu[CA_MOM0].index = 43;
        sys_ecu[CA_MOM1].index = 44;
        sys_ecu[CA_MOM2].index = 45;
        sys_ecu[CA_MOM3].index = 46;
        sys_ecu[CA_GEN0].index = 47;
        sys_ecu[CA_GEN1].index = 48;
        sys_ecu[CA_TLS0].msg = msg_tls;
        sys_ecu[CA_TLS1].msg = msg_tls;
        sys_ecu[CA_VSL0].msg = msg_vsl;
        sys_ecu[CA_VSL1].msg = msg_vsl;
        sys_ecu[CA_PSU].msg = msg_psu;
        sys_ecu[CA_SWH0].msg = msg_swh;
        sys_ecu[CA_SWH1].msg = msg_swh;
        sys_ecu[CA_SWH2].msg = msg_swh;
        sys_ecu[CA_SWH3].msg = msg_swh;
        sys_ecu[CA_RSE0].msg = msg_rse;
        sys_ecu[CA_RSE1].msg = msg_rse;
        sys_ecu[CA_RSE2].msg = msg_rse;
        sys_ecu[CA_RSE3].msg = msg_rse;
        sys_ecu[CA_SWV0].msg = msg_swv;
        sys_ecu[CA_SWV1].msg = msg_swv;
        sys_ecu[CA_SWV2].msg = msg_swv;
        sys_ecu[CA_SWV3].msg = msg_swv;
        sys_ecu[CA_PRP0].msg = msg_prp;
        sys_ecu[CA_PRP1].msg = msg_prp;
        sys_ecu[CA_PRP2].msg = msg_prp;
        sys_ecu[CA_PRP3].msg = msg_prp;
        sys_ecu[CA_X0].msg = msg_xyz;
        sys_ecu[CA_X1].msg = msg_xyz;
        sys_ecu[CA_Y0].msg = msg_xyz;
        sys_ecu[CA_Y1].msg = msg_xyz;
        sys_ecu[CA_Y2].msg = msg_xyz;
        sys_ecu[CA_Y3].msg = msg_xyz;
        sys_ecu[CA_Z0].msg = msg_xyz;
        sys_ecu[CA_Z1].msg = msg_xyz;
        sys_ecu[CA_SHDF0].msg = msg_shd;
        sys_ecu[CA_SHDF1].msg = msg_shd;
        sys_ecu[CA_SHDF2].msg = msg_shd;
        sys_ecu[CA_SHDF3].msg = msg_shd;
        sys_ecu[CA_SHDB0].msg = msg_shd;
        sys_ecu[CA_SHDB1].msg = msg_shd;
        sys_ecu[CA_SHDB2].msg = msg_shd;
        sys_ecu[CA_SHDB3].msg = msg_shd;
        sys_ecu[CA_SHDS0].msg = msg_shd;
        sys_ecu[CA_SHDS1].msg = msg_shd;
        sys_ecu[CA_SHDS2].msg = msg_shd;
        sys_ecu[CA_SHDS3].msg = msg_shd;
        sys_ecu[CA_SHDT].msg = msg_shd;
        sys_ecu[CA_MOM0].msg = msg_mom;
        sys_ecu[CA_MOM1].msg = msg_mom;
        sys_ecu[CA_MOM2].msg = msg_mom;
        sys_ecu[CA_MOM3].msg = msg_mom;
        sys_ecu[CA_GEN0].msg = msg_gen;
        sys_ecu[CA_GEN1].msg = msg_gen;
}
