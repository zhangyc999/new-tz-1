#include "can.h"
#include "canaddr.h"
#include "cmd.h"
#include "data.h"
#include "type.h"
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
extern DATA sys_data;
extern ECU sys_ecu[256];

void t_core(int period)
{
        int recv[2];
        int send[2] = {taskIdSelf(), 0};
        u8 swh0 = 0;
        u8 swh1 = 1;
        u8 swh2 = 2;
        u8 swh3 = 3;
        u8 swv0 = 8;
        u8 swv1 = 9;
        u8 swv2 = 10;
        u8 swv3 = 11;
        u8 prp0 = 12;
        u8 prp1 = 13;
        u8 prp2 = 14;
        u8 prp3 = 15;
        u8 x0 = 16;
        u8 x1 = 17;
        u8 y0 = 18;
        u8 y1 = 19;
        u8 y2 = 20;
        u8 y3 = 21;
        u8 z0 = 22;
        u8 z1 = 23;
        u8 shdf0 = 24;
        u8 shdf1 = 25;
        u8 shdf2 = 26;
        u8 shdf3 = 27;
        u8 shdb0 = 28;
        u8 shdb1 = 29;
        u8 shdb2 = 30;
        u8 shdb3 = 31;
        u8 shds0 = 32;
        u8 shds1 = 33;
        u8 shds2 = 34;
        u8 shds3 = 35;
        u8 shdt = 36;
        u8 mom0 = 37;
        u8 mom1 = 38;
        u8 mom2 = 39;
        u8 mom3 = 40;
        CMD *cmd;
        for (;;) {
                if (sys_data.psu.v24.leg0 && sys_data.psu.v500.leg0 &&
                    sys_data.psu.v24.leg1 && sys_data.psu.v500.leg1 &&
                    sys_data.psu.v24.leg2 && sys_data.psu.v500.leg2 &&
                    sys_data.psu.v24.leg3 && sys_data.psu.v500.leg3) {
                        taskResume(tid_swh);
                        taskResume(tid_swv);
                        taskResume(tid_prp);
                } else {
                        taskSuspend(tid_swh);
                        taskSuspend(tid_swv);
                        taskSuspend(tid_prp);
                        if (!sys_data.psu.v24.leg0) {
                                memset(&sys_data.srv[swh0].fault, 0, 1);
                                memset(&sys_data.srv[swv0].fault, 0, 1);
                                memset(&sys_data.srv[prp0].fault, 0, 1);
                        }
                        if (!sys_data.psu.v24.leg1) {
                                memset(&sys_data.srv[swh1].fault, 0, 1);
                                memset(&sys_data.srv[swv1].fault, 0, 1);
                                memset(&sys_data.srv[prp1].fault, 0, 1);
                        }
                        if (!sys_data.psu.v24.leg2) {
                                memset(&sys_data.srv[swh2].fault, 0, 1);
                                memset(&sys_data.srv[swv2].fault, 0, 1);
                                memset(&sys_data.srv[prp2].fault, 0, 1);
                        }
                        if (!sys_data.psu.v24.leg3) {
                                memset(&sys_data.srv[swh3].fault, 0, 1);
                                memset(&sys_data.srv[swv3].fault, 0, 1);
                                memset(&sys_data.srv[prp3].fault, 0, 1);
                        }
                }
        }
        if (sys_data.psu.v24.xyzf && sys_data.psu.v500.xyzf &&
            sys_data.psu.v24.xyzb && sys_data.psu.v500.xyzb) {
                taskResume(tid_xyz);
        } else {
                taskSuspend(tid_xyz);
                if (!sys_data.psu.v24.xyzf) {
                        memset(&sys_data.srv[x0].fault, 0, 1);
                        memset(&sys_data.srv[y0].fault, 0, 1);
                        memset(&sys_data.srv[y1].fault, 0, 1);
                        memset(&sys_data.srv[z0].fault, 0, 1);
                }
                if (!sys_data.psu.v24.xyzb) {
                        memset(&sys_data.srv[x1].fault, 0, 1);
                        memset(&sys_data.srv[y2].fault, 0, 1);
                        memset(&sys_data.srv[y3].fault, 0, 1);
                        memset(&sys_data.srv[z1].fault, 0, 1);
                }
        }
        if (sys_data.psu.v24.shdf && sys_data.psu.v500.shdf ||
            sys_data.psu.v24.shdb && sys_data.psu.v500.shdb ||
            sys_data.psu.v24.shdst && sys_data.psu.v500.shdst)
                taskResume(tid_shd);
        else
                taskSuspend(tid_shd);
        if (!sys_data.psu.v24.shdf) {
                memset(&sys_data.srv[shdf0].fault, 0, 1);
                memset(&sys_data.srv[shdf1].fault, 0, 1);
                memset(&sys_data.srv[shdf2].fault, 0, 1);
                memset(&sys_data.srv[shdf3].fault, 0, 1);
        }
        if (!sys_data.psu.v24.shdb) {
                memset(&sys_data.srv[shdb0].fault, 0, 1);
                memset(&sys_data.srv[shdb1].fault, 0, 1);
                memset(&sys_data.srv[shdb2].fault, 0, 1);
                memset(&sys_data.srv[shdb3].fault, 0, 1);
        }
        if (!sys_data.psu.v24.shdst) {
                memset(&sys_data.srv[shdt].fault, 0, 1);
                memset(&sys_data.srv[shds0].fault, 0, 1);
                memset(&sys_data.srv[shds1].fault, 0, 1);
                memset(&sys_data.srv[shds2].fault, 0, 1);
                memset(&sys_data.srv[shds3].fault, 0, 1);
        }
        if (sys_data.psu.v24.mom && sys_data.psu.v500.mom) {
                taskResume(tid_mom);
        } else {
                taskSuspend(tid_mom);
                if (!sys_data.psu.v24.mom) {
                        memset(&sys_data.srv[mom0].fault, 0, 1);
                        memset(&sys_data.srv[mom1].fault, 0, 1);
                        memset(&sys_data.srv[mom2].fault, 0, 1);
                        memset(&sys_data.srv[mom3].fault, 0, 1);
                }
        }
        if (8 == msgQReceive(msg_core, (str)recv, 8, period)) {
                cmd = (CMD *)recv[1];
                switch (cmd->dev) {
                case CMD_DEV_TLS:
                        send[1] = (int)cmd;
                        msgQSend(msg_tls, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_VSL:
                        send[1] = (int)cmd;
                        msgQSend(msg_vsl, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_psu, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_PSU:
                        send[1] = (int)cmd;
                        msgQSend(msg_psu, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_SWH:
                        send[1] = (int)cmd;
                        msgQSend(msg_psu, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_swh, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_mom, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_RSE:
                        send[1] = (int)cmd;
                        msgQSend(msg_psu, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_rse, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_SWV:
                        send[1] = (int)cmd;
                        msgQSend(msg_psu, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_swv, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_PRP:
                        send[1] = (int)cmd;
                        msgQSend(msg_psu, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_prp, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_XYZ:
                        send[1] = (int)cmd;
                        msgQSend(msg_psu, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_xyz, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_SHD:
                        send[1] = (int)cmd;
                        msgQSend(msg_psu, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_shd, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_SRV_ALL:
                        send[1] = (int)cmd;
                        msgQSend(msg_swh, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_rse, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_swv, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_prp, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_xyz, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_shd, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_mom, (str)send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                default:
                        break;
                }
        } else {
                /* timeout */
        }
}
