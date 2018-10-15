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
                struct cmd cmd;
        } send;
        send.tid = taskIdSelf();
        send.cmd.src = 0xcc;
        send.cmd.dev = CMD_DEV_PSU;
        send.cmd.mode = CMD_MODE_STUPID;
        send.cmd.act = CMD_ACT_PSU_ON;
        send.cmd.data.psu.toggle
        msgQSend(msg_psu, /*(char *)&send*/, 8, NO_WAIT, MSG_PRI_NORMAL);
        taskResume(tid_swh);
        taskResume(tid_swv);
        taskResume(tid_prp);
        taskDelay(100);
        taskSuspend(tid_swh);
        taskSuspend(tid_swv);
        taskSuspend(tid_prp);
        msgQSend(msg_psu, /*(char *)&send*/, 8, NO_WAIT, MSG_PRI_NORMAL);
        taskResume(tid_xyz);
        taskDelay(100);
        taskSuspend(tid_xyz);
        msgQSend(msg_psu, /*(char *)&send*/, 8, NO_WAIT, MSG_PRI_NORMAL);
        taskResume(tid_shd);
        taskDelay(100);
        taskSuspend(tid_shd);
        msgQSend(msg_psu, /*(char *)&send*/, 8, NO_WAIT, MSG_PRI_NORMAL);
        taskResume(tid_mom);
        taskDelay(100);
        taskSuspend(tid_mom);
        msgQSend(msg_psu, /*(char *)&send*/, 8, NO_WAIT, MSG_PRI_NORMAL);
        taskResume(tid_vsl);
        taskDelay(100);
        taskSuspend(tid_vsl);
        sys_data.misc.boot = 1;
        for (;;) {
                if (8 == msgQReceive(msg_core, (char *)&recv, 8, period)) {
                        switch (recv.p->dev) {
                        case CMD_DEV_TLS:
                                send.cmd = *recv.p;
                                send.cmd.src = 0xec;
                                msgQSend(msg_tls, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_VSL:
                                send.cmd = *recv.p;
                                send.cmd.src = 0xec;
                                msgQSend(msg_vsl, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_PSU:
                                send.cmd = *recv.p;
                                send.cmd.src = 0xec;
                                msgQSend(msg_psu, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_SWH:
                                send.cmd = *recv.p;
                                send.cmd.src = 0xec;
                                msgQSend(msg_swh, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_mom, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_RSE:
                                send.cmd = *recv.p;
                                send.cmd.src = 0xec;
                                msgQSend(msg_rse, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_SWV:
                                send.cmd = *recv.p;
                                send.cmd.src = 0xec;
                                msgQSend(msg_swv, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_PRP:
                                send.cmd = *recv.p;
                                send.cmd.src = 0xec;
                                msgQSend(msg_prp, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_XYZ:
                                send.cmd = *recv.p;
                                send.cmd.src = 0xec;
                                msgQSend(msg_xyz, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_DEV_SHD:
                                send.cmd = *recv.p;
                                send.cmd.src = 0xec;
                                msgQSend(msg_shd, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case CMD_SRV_ALL:
                                send.cmd = *recv.p;
                                send.cmd.src = 0xec;
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
