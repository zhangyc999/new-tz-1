#include "can.h"
#include "canaddr.h"
#include "cmd.h"
#include "data.h"
#include "vx.h"

extern int tid_vsl;
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
        unsigned stamp;
        int swh = 0;
        int rse = 0;
        int swv = 0;
        int prp = 0;
        int xyz = 0;
        int shd = 0;
        int mom = 0;
        int force = 0;
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
        cmd.mode = CMD_MODE_PSU_V24;
        sys_data.misc.boot = 1;
        /* 开始：四支腿（共12个节点）开机自检 */
        cmd.act = CMD_ACT_PSU_ON;
        memset(&cmd.data.psu.toggle, 0, sizeof(cmd.data.psu.toggle));
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
        memset(&cmd.data.psu.toggle, 0, sizeof(cmd.data.psu.toggle));
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
        memset(&cmd.data.psu.toggle, 0, sizeof(cmd.data.psu.toggle));
        cmd.data.psu.toggle.shdb = 1;
        cmd.data.psu.toggle.shdf = 1;
        cmd.data.psu.toggle.shdst = 1;
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
        memset(&cmd.data.psu.toggle, 0, sizeof(cmd.data.psu.toggle));
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
                taskDelay(period);
                if (!force) {
                        if (taskIsSuspended(tid_swh) &&
                            taskIsSuspended(tid_swv) &&
                            taskIsSuspended(tid_prp)) {
                                if (sys_data.psu.v500.leg0 ||
                                    sys_data.psu.v500.leg1 ||
                                    sys_data.psu.v500.leg2 ||
                                    sys_data.psu.v500.leg3) {
                                        cmd.dev = CMD_DEV_PSU;
                                        cmd.mode = CMD_MODE_PSU_V500;
                                        cmd.act = CMD_ACT_PSU_OFF;
                                        memset(&cmd.data.psu.toggle, 0, sizeof(cmd.data.psu.toggle));
                                        cmd.data.psu.toggle.leg0 = 1;
                                        cmd.data.psu.toggle.leg1 = 1;
                                        cmd.data.psu.toggle.leg2 = 1;
                                        cmd.data.psu.toggle.leg3 = 1;
                                        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                        continue;
                                }
                                if (sys_data.psu.v24.leg0 ||
                                    sys_data.psu.v24.leg1 ||
                                    sys_data.psu.v24.leg2 ||
                                    sys_data.psu.v24.leg3) {
                                        cmd.dev = CMD_DEV_PSU;
                                        cmd.mode = CMD_MODE_PSU_V24;
                                        cmd.act = CMD_ACT_PSU_OFF;
                                        memset(&cmd.data.psu.toggle, 0, sizeof(cmd.data.psu.toggle));
                                        cmd.data.psu.toggle.leg0 = 1;
                                        cmd.data.psu.toggle.leg1 = 1;
                                        cmd.data.psu.toggle.leg2 = 1;
                                        cmd.data.psu.toggle.leg3 = 1;
                                        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                        continue;
                                }
                        }
                        if (taskIsSuspended(tid_xyz)) {
                                if (sys_data.psu.v500.xyzb ||
                                    sys_data.psu.v500.xyzf) {
                                        cmd.dev = CMD_DEV_PSU;
                                        cmd.mode = CMD_MODE_PSU_V500;
                                        cmd.act = CMD_ACT_PSU_OFF;
                                        memset(&cmd.data.psu.toggle, 0, sizeof(cmd.data.psu.toggle));
                                        cmd.data.psu.toggle.xyzb = 1;
                                        cmd.data.psu.toggle.xyzf = 1;
                                        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                        continue;
                                }
                                if (sys_data.psu.v24.xyzb ||
                                    sys_data.psu.v24.xyzf) {
                                        cmd.dev = CMD_DEV_PSU;
                                        cmd.mode = CMD_MODE_PSU_V24;
                                        cmd.act = CMD_ACT_PSU_OFF;
                                        memset(&cmd.data.psu.toggle, 0, sizeof(cmd.data.psu.toggle));
                                        cmd.data.psu.toggle.xyzb = 1;
                                        cmd.data.psu.toggle.xyzf = 1;
                                        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                        continue;
                                }
                        }
                        if (taskIsSuspended(tid_shd)) {
                                if (sys_data.psu.v500.shdb ||
                                    sys_data.psu.v500.shdf ||
                                    sys_data.psu.v500.shdst) {
                                        cmd.dev = CMD_DEV_PSU;
                                        cmd.mode = CMD_MODE_PSU_V500;
                                        cmd.act = CMD_ACT_PSU_OFF;
                                        memset(&cmd.data.psu.toggle, 0, sizeof(cmd.data.psu.toggle));
                                        cmd.data.psu.toggle.shdb = 1;
                                        cmd.data.psu.toggle.shdf = 1;
                                        cmd.data.psu.toggle.shdst = 1;
                                        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                        continue;
                                }
                                if (sys_data.psu.v24.shdb ||
                                    sys_data.psu.v24.shdf ||
                                    sys_data.psu.v24.shdst) {
                                        cmd.dev = CMD_DEV_PSU;
                                        cmd.mode = CMD_MODE_PSU_V24;
                                        cmd.act = CMD_ACT_PSU_OFF;
                                        memset(&cmd.data.psu.toggle, 0, sizeof(cmd.data.psu.toggle));
                                        cmd.data.psu.toggle.shdb = 1;
                                        cmd.data.psu.toggle.shdf = 1;
                                        cmd.data.psu.toggle.shdst = 1;
                                        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                        continue;
                                }
                        }
                        if (taskIsSuspended(tid_mom)) {
                                if (sys_data.psu.v500.mom) {
                                        cmd.dev = CMD_DEV_PSU;
                                        cmd.mode = CMD_MODE_PSU_V500;
                                        cmd.act = CMD_ACT_PSU_OFF;
                                        memset(&cmd.data.psu.toggle, 0, sizeof(cmd.data.psu.toggle));
                                        cmd.data.psu.toggle.mom = 1;
                                        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                        continue;
                                }
                                if (sys_data.psu.v24.mom) {
                                        cmd.dev = CMD_DEV_PSU;
                                        cmd.mode = CMD_MODE_PSU_V24;
                                        cmd.act = CMD_ACT_PSU_OFF;
                                        memset(&cmd.data.psu.toggle, 0, sizeof(cmd.data.psu.toggle));
                                        cmd.data.psu.toggle.mom = 1;
                                        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                        continue;
                                }
                        }
                }
                if (sys_data.misc.bus0 > 1 || sys_data.misc.bus1 > 1) {
                        taskSuspend(tid_swh);
                        taskSuspend(tid_rse);
                        taskSuspend(tid_swv);
                        taskSuspend(tid_prp);
                        taskSuspend(tid_xyz);
                        taskSuspend(tid_shd);
                        taskSuspend(tid_mom);
                } else if (8 == msgQReceive(msg_core, (char *)&recv, 8, NO_WAIT)) {
                        switch (recv.p->dev) {
                        case CMD_DEV_TLS:
                                force = 0;
                                cmd = *recv.p;
                                msgQSend(msg_tls, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_VSL:
                                force = 0;
                                cmd = *recv.p;
                                msgQSend(msg_vsl, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_PSU:
                                force = 1;
                                cmd = *recv.p;
                                msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_SWH:
                                force = 0;
                                if (taskIsSuspended(tid_swh)) {
                                        if (!swh) {
                                                sys_data.online[0] += 20;
                                                sys_data.online[1] += 20;
                                                swh = 1;
                                                continue;
                                        }
                                        taskResume(tid_swh);
                                        swh = 0;
                                }
                                cmd = *recv.p;
                                cmd.src = 0xcc;
                                msgQSend(msg_swh, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_RSE:
                                force = 0;
                                if (taskIsSuspended(tid_rse)) {
                                        if (!rse) {
                                                sys_data.online[1] += 40;
                                                rse = 1;
                                                continue;
                                        }
                                        taskResume(tid_rse);
                                        rse = 0;
                                }
                                cmd = *recv.p;
                                cmd.src = 0xcc;
                                msgQSend(msg_rse, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_SWV:
                                force = 0;
                                if (taskIsSuspended(tid_swv)) {
                                        if (!swv) {
                                                sys_data.online[0] += 20;
                                                sys_data.online[1] += 20;
                                                swv = 1;
                                                continue;
                                        }
                                        taskResume(tid_swv);
                                        swv = 0;
                                }
                                cmd = *recv.p;
                                cmd.src = 0xcc;
                                msgQSend(msg_swv, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_PRP:
                                force = 0;
                                if (taskIsSuspended(tid_prp)) {
                                        if (!prp) {
                                                sys_data.online[0] += 20;
                                                sys_data.online[1] += 20;
                                                prp = 1;
                                                continue;
                                        }
                                        taskResume(tid_prp);
                                        prp = 0;
                                }
                                cmd = *recv.p;
                                cmd.src = 0xcc;
                                msgQSend(msg_prp, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_XYZ:
                                force = 0;
                                if (taskIsSuspended(tid_xyz)) {
                                        if (!xyz) {
                                                sys_data.online[0] += 40;
                                                sys_data.online[1] += 40;
                                                xyz = 1;
                                                continue;
                                        }
                                        taskResume(tid_xyz);
                                        xyz = 0;
                                }
                                cmd = *recv.p;
                                cmd.src = 0xcc;
                                msgQSend(msg_xyz, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_SHD:
                                force = 0;
                                if (taskIsSuspended(tid_shd)) {
                                        if (!shd) {
                                                sys_data.online[0] += 40;
                                                sys_data.online[1] += 80;
                                                shd = 1;
                                                continue;
                                        }
                                        taskResume(tid_shd);
                                        shd = 0;
                                }
                                cmd = *recv.p;
                                cmd.src = 0xcc;
                                msgQSend(msg_shd, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_MOM:
                                force = 0;
                                if (taskIsSuspended(tid_mom)) {
                                        if (!mom) {
                                                sys_data.online[0] += 40;
                                                mom = 1;
                                                continue;
                                        }
                                        taskResume(tid_mom);
                                        mom = 0;
                                }
                                cmd = *recv.p;
                                cmd.src = 0xcc;
                                msgQSend(msg_mom, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_SRV_ALL:
                                force = 0;
                                cmd = *recv.p;
                                cmd.src = 0xcc;
                                if (!taskIsSuspended(tid_swh))
                                        msgQSend(msg_swh, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                if (!taskIsSuspended(tid_rse))
                                        msgQSend(msg_rse, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                if (!taskIsSuspended(tid_swv))
                                        msgQSend(msg_swv, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                if (!taskIsSuspended(tid_prp))
                                        msgQSend(msg_prp, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                if (!taskIsSuspended(tid_xyz))
                                        msgQSend(msg_xyz, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                if (!taskIsSuspended(tid_shd))
                                        msgQSend(msg_shd, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                if (!taskIsSuspended(tid_mom))
                                        msgQSend(msg_mom, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        default:
                                break;
                        }
                }
        }
}
