#include "flag.h"
#include "psu.h"
#include "type.h"
#include "udp.h"
#include "vx.h"

extern int tid_mom;
extern int tid_swh;
extern int tid_rse;
extern int tid_swv;
extern int tid_prp;
extern int tid_sdt;
extern int tid_xy;
extern int tid_z;

extern MSG_Q_ID msg_core;
extern MSG_Q_ID msg_vsl;
extern MSG_Q_ID msg_psu;
extern MSG_Q_ID msg_mom;
extern MSG_Q_ID msg_swh;
extern MSG_Q_ID msg_rse;
extern MSG_Q_ID msg_swv;
extern MSG_Q_ID msg_prp;
extern MSG_Q_ID msg_sdt;
extern MSG_Q_ID msg_xy;
extern MSG_Q_ID msg_z;

extern SYS_FLAG flag;

void t_core(int period)
{
        UDP_RX buf;
        for (;;) {
                if ((flag.psu[1] & PSU_MOM) == PSU_MOM)
                        taskResume(tid_mom);
                else
                        taskSuspend(tid_mom);
                if ((flag.psu[1] & PSU_LEG) == PSU_LEG) {
                        taskResume(tid_swh);
                        taskResume(tid_swv);
                        taskResume(tid_prp);
                } else {
                        taskSuspend(tid_swh);
                        taskSuspend(tid_swv);
                        taskSuspend(tid_prp);
                }
                if ((flag.psu[1] & PSU_SDTS) == PSU_SDTS)
                        taskResume(tid_sdt);
                else
                        taskSuspend(tid_sdt);
                if ((flag.psu[1] & PSU_XYZ) == PSU_XYZ) {
                        taskResume(tid_xy);
                        taskResume(tid_z);
                } else {
                        taskSuspend(tid_xy);
                        taskSuspend(tid_z);
                }
                if (ERROR != msgQReceive(msg_core, (str)&buf, sizeof(buf), period)) {
                        switch (buf.cmd[0] & UMASK_UDP_ACT) {
                        case UDP_ACT_IDLE:
                                break;
                        case UDP_ACT_PSU:
                                msgQSend(msg_psu, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case UDP_ACT_SWH:
                                msgQSend(msg_psu, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_mom, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_swh, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case UDP_ACT_RSE:
                                msgQSend(msg_psu, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_rse, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case UDP_ACT_SWV:
                                msgQSend(msg_psu, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_swv, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case UDP_ACT_PRP:
                                msgQSend(msg_psu, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_prp, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case UDP_ACT_SDT:
                                msgQSend(msg_psu, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_sdt, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case UDP_ACT_XY:
                                msgQSend(msg_vsl, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_psu, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_xy, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case UDP_ACT_Z:
                                msgQSend(msg_psu, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_z, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        default:
                                break;
                        }
                } else {
                }
        }
}
