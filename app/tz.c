#include "can.h"
#include "canaddr.h"
#include "type.h"
#include "vx.h"

extern void t_core(void);
extern void t_can(void);
extern void t_tls(void);
extern void t_vsl(void);
extern void t_psu(void);
extern void t_swh(void);
extern void t_rse(void);
extern void t_swv(void);
extern void t_prp(void);
extern void t_xyz(void);
extern void t_shd(void);
extern void t_mom(void);
extern void t_dbg(void);
extern void udp_server(void);

int tid_core;
int tid_can;
int tid_tls;
int tid_vsl;
int tid_psu;
int tid_swh;
int tid_rse;
int tid_swv;
int tid_prp;
int tid_xyz;
int tid_shd;
int tid_mom;
int tid_dbg;

MSG_Q_ID msg_core;
MSG_Q_ID msg_can[2][3];
MSG_Q_ID msg_udp;
MSG_Q_ID msg_tls;
MSG_Q_ID msg_vsl;
MSG_Q_ID msg_psu;
MSG_Q_ID msg_swh;
MSG_Q_ID msg_rse;
MSG_Q_ID msg_swv;
MSG_Q_ID msg_prp;
MSG_Q_ID msg_xyz;
MSG_Q_ID msg_shd;
MSG_Q_ID msg_mom;
MSG_Q_ID msg_gen;
MSG_Q_ID msg_dbg;

DATA sys_data;

ECU sys_ecu[255];

static void ecu_init(void)

void tz(void)
{
        msg_core = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_can[0][0] = msgQCreate(128, sizeof(CAN), MSG_Q_FIFO);
        msg_can[0][1] = msgQCreate(128, sizeof(CAN), MSG_Q_FIFO);
        msg_can[0][2] = msgQCreate(128, sizeof(CAN), MSG_Q_FIFO);
        msg_can[1][0] = msgQCreate(128, sizeof(CAN), MSG_Q_FIFO);
        msg_can[1][1] = msgQCreate(128, sizeof(CAN), MSG_Q_FIFO);
        msg_can[1][2] = msgQCreate(128, sizeof(CAN), MSG_Q_FIFO);
        msg_udp = msgQCreate(128, sizeof(CAN), MSG_Q_FIFO);
        msg_tls = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_vsl = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_psu = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_swh = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_rse = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_swv = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_prp = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_xyz = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_shd = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_mom = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_gen = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_dbg = msgQCreate(128, sizeof(CAN), MSG_Q_FIFO);
        lstLibInit();
        ecu_init();
        sysClkRateSet(100);
        tid_core = taskSpawn("CORE", 40, VX_FP_TASK, 20000, (FUNCPTR)t_core, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_can = taskSpawn("CAN", 40, VX_FP_TASK, 20000, (FUNCPTR)t_can, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_tls = taskSpawn("TLS", 40, VX_FP_TASK, 20000, (FUNCPTR)t_tls, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_vsl = taskSpawn("VSL", 90, VX_FP_TASK, 20000, (FUNCPTR)t_vsl, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_psu = taskSpawn("PSU", 90, VX_FP_TASK, 20000, (FUNCPTR)t_psu, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_swh = taskSpawn("SWH", 90, VX_FP_TASK, 20000, (FUNCPTR)t_swh, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_rse = taskSpawn("RSE", 90, VX_FP_TASK, 20000, (FUNCPTR)t_rse, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_swv = taskSpawn("SWV", 90, VX_FP_TASK, 20000, (FUNCPTR)t_swv, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_prp = taskSpawn("PRP", 90, VX_FP_TASK, 20000, (FUNCPTR)t_prp, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_xyz = taskSpawn("XYZ", 90, VX_FP_TASK, 20000, (FUNCPTR)t_xy, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_shd = taskSpawn("SHD", 90, VX_FP_TASK, 20000, (FUNCPTR)t_shd, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_mom = taskSpawn("MOM", 90, VX_FP_TASK, 20000, (FUNCPTR)t_mom, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_dbg = taskSpawn("DBG", 100, VX_FP_TASK, 20000, (FUNCPTR)t_dbg, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        udp_server();
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
        sys_ecu[CA_SHDT].index = 30;
        sys_ecu[CA_SHDS0].index = 31;
        sys_ecu[CA_SHDS1].index = 32;
        sys_ecu[CA_SHDS2].index = 33;
        sys_ecu[CA_SHDS3].index = 34;
        sys_ecu[CA_SHDF0].index = 35;
        sys_ecu[CA_SHDF1].index = 36;
        sys_ecu[CA_SHDF2].index = 37;
        sys_ecu[CA_SHDF3].index = 38;
        sys_ecu[CA_SHDB0].index = 39;
        sys_ecu[CA_SHDB1].index = 40;
        sys_ecu[CA_SHDB2].index = 41;
        sys_ecu[CA_SHDB3].index = 42;
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
        sys_ecu[CA_SHDT].msg = msg_shd;
        sys_ecu[CA_SHDS0].msg = msg_shd;
        sys_ecu[CA_SHDS1].msg = msg_shd;
        sys_ecu[CA_SHDS2].msg = msg_shd;
        sys_ecu[CA_SHDS3].msg = msg_shd;
        sys_ecu[CA_SHDF0].msg = msg_shd;
        sys_ecu[CA_SHDF1].msg = msg_shd;
        sys_ecu[CA_SHDF2].msg = msg_shd;
        sys_ecu[CA_SHDF3].msg = msg_shd;
        sys_ecu[CA_SHDB0].msg = msg_shd;
        sys_ecu[CA_SHDB1].msg = msg_shd;
        sys_ecu[CA_SHDB2].msg = msg_shd;
        sys_ecu[CA_SHDB3].msg = msg_shd;
        sys_ecu[CA_MOM0].msg = msg_mom;
        sys_ecu[CA_MOM1].msg = msg_mom;
        sys_ecu[CA_MOM2].msg = msg_mom;
        sys_ecu[CA_MOM3].msg = msg_mom;
        sys_ecu[CA_GEN0].msg = msg_gen;
        sys_ecu[CA_GEN1].msg = msg_gen;
}
