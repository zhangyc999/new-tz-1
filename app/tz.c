#include "can.h"
#include "flag.h"
#include "type.h"
#include "vx.h"

extern void t_core(int period);
extern void t_can(int period);
extern void t_tls(int period);
extern void t_vsl(int period_slow, int period_fast, int prio_low, int prio_high);
extern void t_psu(int period_slow, int period_fast, int prio_low, int prio_high);
extern void t_mom(int period_slow, int period_fast, int prio_low, int prio_high);
extern void t_swh(int period_slow, int period_fast, int prio_low, int prio_high);
extern void t_rse(int period_slow, int period_fast, int prio_low, int prio_high);
extern void t_swv(int period_slow, int period_fast, int prio_low, int prio_high);
extern void t_prp(int period_slow, int period_fast, int prio_low, int prio_high);
extern void t_sdt(int period_slow, int period_fast, int prio_low, int prio_high);
extern void t_xy(int period_slow, int period_fast, int prio_low, int prio_high);
extern void t_z(int period_slow, int period_fast, int prio_low, int prio_high);
extern void t_dbg(int period);
extern void udp_server(void);

int tid_core;
int tid_can;
int tid_tls;
int tid_vsl;
int tid_psu;
int tid_mom;
int tid_swh;
int tid_rse;
int tid_swv;
int tid_prp;
int tid_sdt;
int tid_xy;
int tid_z;
int tid_dbg;

MSG_Q_ID msg_core;
MSG_Q_ID msg_can[2][3];
MSG_Q_ID msg_udp;
MSG_Q_ID msg_tls;
MSG_Q_ID msg_vsl;
MSG_Q_ID msg_psu;
MSG_Q_ID msg_mom;
MSG_Q_ID msg_swh;
MSG_Q_ID msg_rse;
MSG_Q_ID msg_swv;
MSG_Q_ID msg_prp;
MSG_Q_ID msg_sdt;
MSG_Q_ID msg_xy;
MSG_Q_ID msg_z;
MSG_Q_ID msg_dbg;

SYS_FLAG flag;

void tz(void)
{
        lstLibInit();
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
        msg_mom = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_swh = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_rse = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_swv = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_prp = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_sdt = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_xy = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_z = msgQCreate(64, sizeof(CAN), MSG_Q_FIFO);
        msg_dbg = msgQCreate(128, sizeof(CAN), MSG_Q_FIFO);
        sysClkRateSet(100);
        tid_core = taskSpawn("CORE", 40, VX_FP_TASK, 20000, (FUNCPTR)t_core, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_can = taskSpawn("CAN", 40, VX_FP_TASK, 20000, (FUNCPTR)t_can, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_tls = taskSpawn("TLS", 40, VX_FP_TASK, 20000, (FUNCPTR)t_tls, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_vsl = taskSpawn("VSL", 90, VX_FP_TASK, 20000, (FUNCPTR)t_vsl, 100, 100, 90, 40, 0, 0, 0, 0, 0, 0);
        tid_psu = taskSpawn("PSU", 90, VX_FP_TASK, 20000, (FUNCPTR)t_psu, 10, 10, 90, 40, 0, 0, 0, 0, 0, 0);
        tid_mom = taskSpawn("MOM", 90, VX_FP_TASK, 20000, (FUNCPTR)t_mom, 100, 10, 90, 40, 0, 0, 0, 0, 0, 0);
        tid_swh = taskSpawn("SWH", 90, VX_FP_TASK, 20000, (FUNCPTR)t_swh, 100, 10, 90, 40, 0, 0, 0, 0, 0, 0);
        tid_rse = taskSpawn("RSE", 90, VX_FP_TASK, 20000, (FUNCPTR)t_rse, 100, 10, 90, 40, 0, 0, 0, 0, 0, 0);
        tid_swv = taskSpawn("SWV", 90, VX_FP_TASK, 20000, (FUNCPTR)t_swv, 100, 10, 90, 40, 0, 0, 0, 0, 0, 0);
        tid_prp = taskSpawn("PRP", 90, VX_FP_TASK, 20000, (FUNCPTR)t_prp, 100, 10, 90, 40, 0, 0, 0, 0, 0, 0);
        tid_sdt = taskSpawn("SDT", 90, VX_FP_TASK, 20000, (FUNCPTR)t_sdt, 100, 10, 90, 40, 0, 0, 0, 0, 0, 0);
        tid_xy = taskSpawn("XY", 90, VX_FP_TASK, 20000, (FUNCPTR)t_xy, 100, 10, 90, 40, 0, 0, 0, 0, 0, 0);
        tid_z = taskSpawn("Z", 90, VX_FP_TASK, 20000, (FUNCPTR)t_z, 100, 10, 90, 40, 0, 0, 0, 0, 0, 0);
        tid_dbg = taskSpawn("DBG", 100, VX_FP_TASK, 20000, (FUNCPTR)t_dbg, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        udp_server();
}
