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

void t_core(void)
{
        int period = sysClkRateGet() / 20;
        u8 swh0 = sys_ecu[CA_SWH0].index - 6;
        u8 swh1 = sys_ecu[CA_SWH1].index - 6;
        u8 swh2 = sys_ecu[CA_SWH2].index - 6;
        u8 swh3 = sys_ecu[CA_SWH3].index - 6;
        u8 swv0 = sys_ecu[CA_SWV0].index - 6;
        u8 swv1 = sys_ecu[CA_SWV1].index - 6;
        u8 swv2 = sys_ecu[CA_SWV2].index - 6;
        u8 swv3 = sys_ecu[CA_SWV3].index - 6;
        u8 prp0 = sys_ecu[CA_PRP0].index - 6;
        u8 prp1 = sys_ecu[CA_PRP1].index - 6;
        u8 prp2 = sys_ecu[CA_PRP2].index - 6;
        u8 prp3 = sys_ecu[CA_PRP3].index - 6;
        u8 x0 = sys_ecu[CA_X0].index - 6;
        u8 x1 = sys_ecu[CA_X1].index - 6;
        u8 y0 = sys_ecu[CA_Y0].index - 6;
        u8 y1 = sys_ecu[CA_Y1].index - 6;
        u8 y2 = sys_ecu[CA_Y2].index - 6;
        u8 y3 = sys_ecu[CA_Y3].index - 6;
        u8 z0 = sys_ecu[CA_Z0].index - 6;
        u8 z1 = sys_ecu[CA_Z1].index - 6;
        u8 shdt = sys_ecu[CA_SHDT].index - 6;
        u8 shds0 = sys_ecu[CA_SHDS0].index - 6;
        u8 shds1 = sys_ecu[CA_SHDS1].index - 6;
        u8 shds2 = sys_ecu[CA_SHDS2].index - 6;
        u8 shds3 = sys_ecu[CA_SHDS3].index - 6;
        u8 shdf0 = sys_ecu[CA_SHDF0].index - 6;
        u8 shdf1 = sys_ecu[CA_SHDF1].index - 6;
        u8 shdf2 = sys_ecu[CA_SHDF2].index - 6;
        u8 shdf3 = sys_ecu[CA_SHDF3].index - 6;
        u8 shdb0 = sys_ecu[CA_SHDB0].index - 6;
        u8 shdb1 = sys_ecu[CA_SHDB1].index - 6;
        u8 shdb2 = sys_ecu[CA_SHDB2].index - 6;
        u8 shdb3 = sys_ecu[CA_SHDB3].index - 6;
        u8 mom0 = sys_ecu[CA_MOM0].index - 6;
        u8 mom1 = sys_ecu[CA_MOM1].index - 6;
        u8 mom2 = sys_ecu[CA_MOM2].index - 6;
        u8 mom3 = sys_ecu[CA_MOM3].index - 6;
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
        if (sys_data.psu.v24.shdts && sys_data.psu.v500.shdts ||
            sys_data.psu.v24.shdf && sys_data.psu.v500.shdf ||
            sys_data.psu.v24.shdb && sys_data.psu.v500.shdb)
                taskResume(tid_shd);
        else
                taskSuspend(tid_shd);
        if (!sys_data.psu.v24.shdts) {
                memset(&sys_data.srv[shdt].fault, 0, 1);
                memset(&sys_data.srv[shds0].fault, 0, 1);
                memset(&sys_data.srv[shds1].fault, 0, 1);
                memset(&sys_data.srv[shds2].fault, 0, 1);
                memset(&sys_data.srv[shds3].fault, 0, 1);
        }
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
        if (4 == msgQReceive(msg_core, (str)&cmd, 4, period)) {
                switch (cmd->dev) {
                case CMD_DEV_TLS:
                        msgQSend(msg_tls, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_VSL:
                        msgQSend(msg_vsl, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_psu, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_PSU:
                        msgQSend(msg_psu, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_SWH:
                        msgQSend(msg_psu, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_swh, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_mom, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_RSE:
                        msgQSend(msg_psu, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_rse, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_SWV:
                        msgQSend(msg_psu, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_swv, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_PRP:
                        msgQSend(msg_psu, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_prp, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_XYZ:
                        msgQSend(msg_psu, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_xyz, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_DEV_SHD:
                        msgQSend(msg_psu, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_shd, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                case CMD_SRV_ALL:
                        msgQSend(msg_swh, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_rse, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_swv, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_prp, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_xyz, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_shd, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        msgQSend(msg_mom, (str)&cmd, 4, NO_WAIT, MSG_PRI_NORMAL);
                        break;
                default:
                        break;
                }
        } else {
                /* timeout */
        }
}
