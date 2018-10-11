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
extern DATA sys_data;
extern ECU sys_ecu[256];

void t_core(int period)
{
        struct {
                int tid;
                CMD *p;
        } recv, send;
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
        send.tid = taskIdSelf();
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
                                memset(&sys_data.srv[swh0].fault, 0, 4);
                                memset(&sys_data.srv[swv0].fault, 0, 4);
                                memset(&sys_data.srv[prp0].fault, 0, 4);
                        }
                        if (!sys_data.psu.v24.leg1) {
                                memset(&sys_data.srv[swh1].fault, 0, 4);
                                memset(&sys_data.srv[swv1].fault, 0, 4);
                                memset(&sys_data.srv[prp1].fault, 0, 4);
                        }
                        if (!sys_data.psu.v24.leg2) {
                                memset(&sys_data.srv[swh2].fault, 0, 4);
                                memset(&sys_data.srv[swv2].fault, 0, 4);
                                memset(&sys_data.srv[prp2].fault, 0, 4);
                        }
                        if (!sys_data.psu.v24.leg3) {
                                memset(&sys_data.srv[swh3].fault, 0, 4);
                                memset(&sys_data.srv[swv3].fault, 0, 4);
                                memset(&sys_data.srv[prp3].fault, 0, 4);
                        }
                }
        }
        if (sys_data.psu.v24.xyzf && sys_data.psu.v500.xyzf &&
            sys_data.psu.v24.xyzb && sys_data.psu.v500.xyzb) {
                taskResume(tid_xyz);
        } else {
                taskSuspend(tid_xyz);
                if (!sys_data.psu.v24.xyzf) {
                        memset(&sys_data.srv[x0].fault, 0, 4);
                        memset(&sys_data.srv[y0].fault, 0, 4);
                        memset(&sys_data.srv[y1].fault, 0, 4);
                        memset(&sys_data.srv[z0].fault, 0, 4);
                }
                if (!sys_data.psu.v24.xyzb) {
                        memset(&sys_data.srv[x1].fault, 0, 4);
                        memset(&sys_data.srv[y2].fault, 0, 4);
                        memset(&sys_data.srv[y3].fault, 0, 4);
                        memset(&sys_data.srv[z1].fault, 0, 4);
                }
        }
        if (sys_data.psu.v24.shdf && sys_data.psu.v500.shdf ||
            sys_data.psu.v24.shdb && sys_data.psu.v500.shdb ||
            sys_data.psu.v24.shdst && sys_data.psu.v500.shdst)
                taskResume(tid_shd);
        else
                taskSuspend(tid_shd);
        if (!sys_data.psu.v24.shdf) {
                memset(&sys_data.srv[shdf0].fault, 0, 4);
                memset(&sys_data.srv[shdf1].fault, 0, 4);
                memset(&sys_data.srv[shdf2].fault, 0, 4);
                memset(&sys_data.srv[shdf3].fault, 0, 4);
        }
        if (!sys_data.psu.v24.shdb) {
                memset(&sys_data.srv[shdb0].fault, 0, 4);
                memset(&sys_data.srv[shdb1].fault, 0, 4);
                memset(&sys_data.srv[shdb2].fault, 0, 4);
                memset(&sys_data.srv[shdb3].fault, 0, 4);
        }
        if (!sys_data.psu.v24.shdst) {
                memset(&sys_data.srv[shdt].fault, 0, 4);
                memset(&sys_data.srv[shds0].fault, 0, 4);
                memset(&sys_data.srv[shds1].fault, 0, 4);
                memset(&sys_data.srv[shds2].fault, 0, 4);
                memset(&sys_data.srv[shds3].fault, 0, 4);
        }
        if (sys_data.psu.v24.mom && sys_data.psu.v500.mom) {
                taskResume(tid_mom);
        } else {
                taskSuspend(tid_mom);
                if (!sys_data.psu.v24.mom) {
                        memset(&sys_data.srv[mom0].fault, 0, 4);
                        memset(&sys_data.srv[mom1].fault, 0, 4);
                        memset(&sys_data.srv[mom2].fault, 0, 4);
                        memset(&sys_data.srv[mom3].fault, 0, 4);
                }
        }
        if (8 == msgQReceive(msg_core, (char *)&recv, 8, period)) {
                switch (recv.p->dev) {
                case CMD_DEV_TLS:
                        send.p = recv.p;
                        msgQSend(msg_tls, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_VSL:
                        send.p = recv.p;
                        msgQSend(msg_vsl, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_psu, (char *)&send, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_PSU:
                        send.p = recv.p;
                        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_SWH:
                        send.p = recv.p;
                        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_swh, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_mom, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_RSE:
                        send.p = recv.p;
                        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_rse, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_SWV:
                        send.p = recv.p;
                        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_swv, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_PRP:
                        send.p = recv.p;
                        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_prp, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_XYZ:
                        send.p = recv.p;
                        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_xyz, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_SHD:
                        send.p = recv.p;
                        msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_shd, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_SRV_ALL:
                        send.p = recv.p;
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
