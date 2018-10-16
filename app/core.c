#include "can.h"
#include "canaddr.h"
#include "cmd.h"
#include "data.h"
#include "vx.h"

extern int tid_swh;
extern int tid_rse;
extern int tid_swv;
extern int tid_prp;
extern int tid_xyz;
extern int tid_shd;
extern int tid_mom;
extern MSG_Q_ID msg_core;
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
extern unsigned ev_core_vsl;
extern unsigned ev_core_swh;
extern unsigned ev_core_rse;
extern unsigned ev_core_swv;
extern unsigned ev_core_prp;
extern unsigned ev_core_xyz;
extern unsigned ev_core_shd;
extern unsigned ev_core_mom;
extern struct data sys_data;

void t_core(int period)
{
        unsigned char swh0 = 0;
        unsigned char swh1 = 1;
        unsigned char swh2 = 2;
        unsigned char swh3 = 3;
        unsigned char swv0 = 8;
        unsigned char swv1 = 9;
        unsigned char swv2 = 10;
        unsigned char swv3 = 11;
        unsigned char prp0 = 12;
        unsigned char prp1 = 13;
        unsigned char prp2 = 14;
        unsigned char prp3 = 15;
        unsigned char x0 = 16;
        unsigned char x1 = 17;
        unsigned char y0 = 18;
        unsigned char y1 = 19;
        unsigned char y2 = 20;
        unsigned char y3 = 21;
        unsigned char z0 = 22;
        unsigned char z1 = 23;
        unsigned char shdf0 = 24;
        unsigned char shdf1 = 25;
        unsigned char shdf2 = 26;
        unsigned char shdf3 = 27;
        unsigned char shdb0 = 28;
        unsigned char shdb1 = 29;
        unsigned char shdb2 = 30;
        unsigned char shdb3 = 31;
        unsigned char shds0 = 32;
        unsigned char shds1 = 33;
        unsigned char shds2 = 34;
        unsigned char shds3 = 35;
        unsigned char shdt = 36;
        unsigned char mom0 = 37;
        unsigned char mom1 = 38;
        unsigned char mom2 = 39;
        unsigned char mom3 = 40;
        struct {
                int tid;
                struct cmd *p;
        } recv;
        struct {
                int tid;
                struct cmd *p;
        } send;
        struct cmd cmd;
        send.tid = taskIdSelf();
        send.p = &cmd;
        cmd.src = 0xcc;
        cmd.dev = CMD_DEV_PSU;
        cmd.mode = CMD_MODE_V24;
        sys_data.misc.boot = 1;
        /* 开始：四支腿（共12个节点）开机自检 */
        cmd.act = CMD_ACT_PSU_ON;
        cmd.data.psu.toggle.leg0 = 1;
        cmd.data.psu.toggle.leg1 = 1;
        cmd.data.psu.toggle.leg2 = 1;
        cmd.data.psu.toggle.leg3 = 1;
        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
        stamp = tickGet();
        taskResume(tid_swh);
        taskResume(tid_swv);
        taskResume(tid_prp);
        eventReceive(ev_core_swh | ev_core_swv | ev_core_prp, EVENTS_WAIT_ALL, 200, NULL);
        taskSuspend(tid_swh);
        taskSuspend(tid_swv);
        taskSuspend(tid_prp);
        cmd.act = CMD_ACT_PSU_OFF;
        if (tickGet() - stamp < period)
                taskDelay(period - tickGet() + stamp);
        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
        taskDelay(period);
        /* 结束：四支腿（共12个节点）开机自检 */
        /* 开始：三轴转载机构（共8个节点）开机自检 */
        cmd.act = CMD_ACT_PSU_ON;
        cmd.data.psu.toggle.leg0 = 0;
        cmd.data.psu.toggle.leg1 = 0;
        cmd.data.psu.toggle.leg2 = 0;
        cmd.data.psu.toggle.leg3 = 0;
        cmd.data.psu.toggle.xyzb = 1;
        cmd.data.psu.toggle.xyzf = 1;
        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
        stamp = tickGet();
        taskResume(tid_xyz);
        eventReceive(ev_core_xyz, EVENTS_WAIT_ALL, 200, NULL);
        taskSuspend(tid_xyz);
        cmd.act = CMD_ACT_PSU_OFF;
        if (tickGet() - stamp < period)
                taskDelay(period - tickGet() + stamp);
        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
        taskDelay(period);
        /* 结束：三轴转载机构（共8个节点）开机自检 */
        /* 开始：防护棚（共12个节点）开机自检 */
        cmd.act = CMD_ACT_PSU_ON;
        cmd.data.psu.toggle.xyzb = 0;
        cmd.data.psu.toggle.xyzf = 0;
        cmd.data.psu.toggle.shdb = 1;
        cmd.data.psu.toggle.shdf = 1;
        cmd.data.psu.toggle.shst = 1;
        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
        stamp = tickGet();
        taskResume(tid_shd);
        eventReceive(ev_core_shd, EVENTS_WAIT_ALL, 200, NULL);
        taskSuspend(tid_shd);
        cmd.act = CMD_ACT_PSU_OFF;
        if (tickGet() - stamp < period)
                taskDelay(period - tickGet() + stamp);
        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
        taskDelay(period);
        /* 结束：防护棚（共12个节点）开机自检 */
        /* 开始：恒力矩、顶升（共4+4个节点）开机自检 */
        cmd.act = CMD_ACT_PSU_ON;
        cmd.data.psu.toggle.shdb = 0;
        cmd.data.psu.toggle.shdf = 0;
        cmd.data.psu.toggle.shst = 0;
        cmd.data.psu.toggle.mom = 1;
        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
        stamp = tickGet();
        taskResume(tid_mom);
        taskResume(tid_rse);
        eventReceive(ev_core_mom | ev_core_rse, EVENTS_WAIT_ALL, 200, NULL);
        taskSuspend(tid_mom);
        taskSuspend(tid_rse);
        cmd.act = CMD_ACT_PSU_OFF;
        if (tickGet() - stamp < period)
                taskDelay(period - tickGet() + stamp);
        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
        taskDelay(period);
        cmd.data.psu.toggle.mom = 0;
        /* 结束：恒力矩、顶升（共4+4个节点）开机自检 */
        /* 开始：视觉定位（共2个节点）开机自检 */
        taskResume(tid_vsl);
        eventReceive(ev_core_vsl, EVENTS_WAIT_ALL, 1000, NULL);
        taskSuspend(tid_vsl);
        /* 结束：视觉定位（共2个节点）开机自检 */
        sys_data.misc.boot = 2;
        for (;;) {
                if (8 == msgQReceive(msg_core, (char *)&recv, 8, period)) {
                        switch (recv.p->dev) {
                        case CMD_DEV_TLS:
                                cmd = *recv.p;
                                msgQSend(msg_tls, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_VSL:
                                cmd = *recv.p;
                                msgQSend(msg_vsl, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_PSU:
                                cmd = *recv.p;
                                msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_SWH:
                                cmd = *recv.p;
                                msgQSend(msg_swh, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_mom, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_RSE:
                                cmd = *recv.p;
                                msgQSend(msg_rse, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_SWV:
                                cmd = *recv.p;
                                msgQSend(msg_swv, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_PRP:
                                cmd = *recv.p;
                                msgQSend(msg_prp, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_XYZ:
                                cmd = *recv.p;
                                msgQSend(msg_xyz, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_SHD:
                                cmd = *recv.p;
                                msgQSend(msg_shd, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_SRV_ALL:
                                cmd = *recv.p;
                                msgQSend(msg_swh, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_rse, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_swv, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_prp, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_xyz, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_shd, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_mom, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        default:
                                break;
                        }
                } else {
                        /* timeout */
                }
        }
}
