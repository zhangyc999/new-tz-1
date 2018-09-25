#include "canaddr.h"
#include "type.h"
#include "udp.h"
#include "vx.h"

extern int tid_swh;
extern int tid_rse;
extern int tid_swv;
extern int tid_prp;
extern int tid_xyz;
extern int tid_shd;
extern int tid_mom;

extern MSG_Q_ID msg_core;
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

extern ECU sys_ecu[255];

void t_core(void)
{
        static int period = sysClkRateGet() / 10;
        static UDP buf;
        static u8 psu = sys_ecu[CA_PSU]->index - 1;
        static u8 swh0 = sys_ecu[CA_SWH0]->index - 1;
        static u8 swh0 = sys_ecu[CA_SWH0]->index - 1;
        static u8 swh0 = sys_ecu[CA_SWH0]->index - 1;
        static u8 swh0 = sys_ecu[CA_SWH0]->index - 1;
        static u8 swv = sys_ecu[CA_SWV]->index - 1;
        static u8 prp = sys_ecu[CA_PRP]->index - 1;
        static u8 xyz = sys_ecu[CA_XYZ]->index - 1;
        static u8 shd = sys_ecu[CA_SHD]->index - 1;
        static u8 mom = sys_ecu[CA_MOM]->index - 1;
        for (;;) {
                if (sys_data.dev[psu].psu.v24.leg0 && sys_data.dev[psu].psu.v500.leg0 &&
                    sys_data.dev[psu].psu.v24.leg1 && sys_data.dev[psu].psu.v500.leg1 &&
                    sys_data.dev[psu].psu.v24.leg2 && sys_data.dev[psu].psu.v500.leg2 &&
                    sys_data.dev[psu].psu.v24.leg3 && sys_data.dev[psu].psu.v500.leg3) {
                        taskResume(tid_swh);
                        taskResume(tid_swv);
                        taskResume(tid_prp);
                } else {
                        taskSuspend(tid_swh);
                        taskSuspend(tid_swv);
                        taskSuspend(tid_prp);
                        memset(&sys_data.dev[swh].srv.fault, 0, 1);
                        memset(&sys_data.dev[swv].srv.fault, 0, 1);
                        memset(&sys_data.dev[prp].srv.fault, 0, 1);
                }
                if (sys_data.dev[psu].psu.v24.xyzf && sys_data.dev[psu].psu.v500.xyzf &&
                    sys_data.dev[psu].psu.v24.xyzb && sys_data.dev[psu].psu.v500.xyzb) {
                        taskResume(tid_xyz);
                } else {
                        taskSuspend(tid_xyz);
                        memset(&sys_data.dev[xyz].srv.fault, 0, 1);
                }
                if (sys_data.dev[psu].psu.v24.shdts && sys_data.dev[psu].psu.v500.shdts ||
                    sys_data.dev[psu].psu.v24.shdf && sys_data.dev[psu].psu.v500.shdf ||
                    sys_data.dev[psu].psu.v24.shdb && sys_data.dev[psu].psu.v500.shdb) {
                        taskResume(tid_shd);
                } else {
                        taskSuspend(tid_shd);
                        memset(&sys_data.dev[shd].srv.fault, 0, 1);
                }
                if (sys_data.dev[psu].psu.v24.mom && sys_data.dev[psu].psu.v500.mom) {
                        taskResume(tid_mom);
                } else {
                        taskSuspend(tid_mom);
                        memset(&sys_data.dev[mom].srv.fault, 0, 1);
                }
                if (ERROR != msgQReceive(msg_core, (str)&buf, sizeof(buf), period)) {
                        switch (buf.cmd[0] & UMASK_UDP_ACT) {
                        case UDP_ACT_IDLE:
                                break;
                        case UDP_ACT_VSL:
                                msgQSend(msg_vsl, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
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
                                msgQSend(msg_shd, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        case UDP_ACT_XYZ:
                                msgQSend(msg_psu, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                msgQSend(msg_xyz, (str)&buf, sizeof(buf), NO_WAIT, MSG_PRI_NORMAL);
                                break;
                        default:
                                break;
                        }
                } else {
                }
        }
}
