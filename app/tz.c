#include "can.h"
#include "type.h"
#include "vx.h"

extern void t_core(int prio_low, int prio_high, int period_slow, int period_fast);
extern void t_can(int prio_low, int prio_high, int period_slow, int period_fast);
extern void t_tls(int prio_low, int prio_high, int period_slow, int period_fast);
extern void t_vsl(int prio_low, int prio_high, int period_slow, int period_fast);
extern void t_psu(int prio_low, int prio_high, int period_slow, int period_fast);
extern void t_mom(int prio_low, int prio_high, int period_slow, int period_fast);
extern void t_swh(int prio_low, int prio_high, int period_slow, int period_fast);
extern void t_rse(int prio_low, int prio_high, int period_slow, int period_fast);
extern void t_swv(int prio_low, int prio_high, int period_slow, int period_fast);
extern void t_prp(int prio_low, int prio_high, int period_slow, int period_fast);
extern void t_shd(int prio_low, int prio_high, int period_slow, int period_fast);
extern void t_xyz(int prio_low, int prio_high, int period_slow, int period_fast);
extern void t_dbg(int prio_low, int prio_high, int period_slow, int period_fast);
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
int tid_shd;
int tid_xyz;
int tid_dbg;

MSG_Q_ID msg_core;
MSG_Q_ID msg_can_tx[2][3];
MSG_Q_ID msg_can_rx;
MSG_Q_ID msg_udp;
MSG_Q_ID msg_tls;
MSG_Q_ID msg_vsl;
MSG_Q_ID msg_psu;
MSG_Q_ID msg_mom;
MSG_Q_ID msg_swh;
MSG_Q_ID msg_rse;
MSG_Q_ID msg_swv;
MSG_Q_ID msg_prp;
MSG_Q_ID msg_shd;
MSG_Q_ID msg_xyz;
MSG_Q_ID msg_dbg;

u32 sys_flag[50];

void tz(void)
{
        lstLibInit();
        msg_core = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_can_tx[0][0] = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_can_tx[0][1] = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_can_tx[0][2] = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_can_tx[1][0] = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_can_tx[1][1] = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_can_tx[1][2] = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_can_rx = msgQCreate(128, sizeof(CAN) - sizeof(NODE), MSG_Q_FIFO);
        msg_udp = msgQCreate(128, 8, MSG_Q_FIFO);
        msg_tls = msgQCreate(64, 8, MSG_Q_FIFO);
        msg_vsl = msgQCreate(64, 8, MSG_Q_FIFO);
        msg_psu = msgQCreate(64, 8, MSG_Q_FIFO);
        msg_mom = msgQCreate(64, 8, MSG_Q_FIFO);
        msg_swh = msgQCreate(64, 8, MSG_Q_FIFO);
        msg_rse = msgQCreate(64, 8, MSG_Q_FIFO);
        msg_swv = msgQCreate(64, 8, MSG_Q_FIFO);
        msg_prp = msgQCreate(64, 8, MSG_Q_FIFO);
        msg_shd = msgQCreate(64, 8, MSG_Q_FIFO);
        msg_xyz = msgQCreate(64, 8, MSG_Q_FIFO);
        msg_dbg = msgQCreate(128, 8, MSG_Q_FIFO);
        sysClkRateSet(100);
        tid_core = taskSpawn("CORE", 90, VX_FP_TASK, 20000, (FUNCPTR)t_core, 90, 40, 1, 1, 0, 0, 0, 0, 0, 0);
        tid_can = taskSpawn("CAN", 90, VX_FP_TASK, 20000, (FUNCPTR)t_can, 90, 40, 1, 1, 0, 0, 0, 0, 0, 0);
        tid_tls = taskSpawn("TLS", 90, VX_FP_TASK, 20000, (FUNCPTR)t_tls, 90, 40, 100, 100, 0, 0, 0, 0, 0, 0);
        tid_vsl = taskSpawn("VSL", 90, VX_FP_TASK, 20000, (FUNCPTR)t_vsl, 90, 40, 100, 100, 0, 0, 0, 0, 0, 0);
        tid_psu = taskSpawn("PSU", 90, VX_FP_TASK, 20000, (FUNCPTR)t_psu, 90, 40, 10, 10, 0, 0, 0, 0, 0, 0);
        tid_mom = taskSpawn("MOM", 90, VX_FP_TASK, 20000, (FUNCPTR)t_mom, 90, 40, 100, 10, 0, 0, 0, 0, 0, 0);
        tid_swh = taskSpawn("SWH", 90, VX_FP_TASK, 20000, (FUNCPTR)t_swh, 90, 40, 100, 10, 0, 0, 0, 0, 0, 0);
        tid_rse = taskSpawn("RSE", 90, VX_FP_TASK, 20000, (FUNCPTR)t_rse, 90, 40, 100, 10, 0, 0, 0, 0, 0, 0);
        tid_swv = taskSpawn("SWV", 90, VX_FP_TASK, 20000, (FUNCPTR)t_swv, 90, 40, 100, 10, 0, 0, 0, 0, 0, 0);
        tid_prp = taskSpawn("PRP", 90, VX_FP_TASK, 20000, (FUNCPTR)t_prp, 90, 40, 100, 10, 0, 0, 0, 0, 0, 0);
        tid_shd = taskSpawn("SHD", 90, VX_FP_TASK, 20000, (FUNCPTR)t_shd, 90, 40, 100, 10, 0, 0, 0, 0, 0, 0);
        tid_xyz = taskSpawn("XYZ", 90, VX_FP_TASK, 20000, (FUNCPTR)t_xyz, 90, 40, 100, 10, 0, 0, 0, 0, 0, 0);
        tid_dbg = taskSpawn("DBG", 100, VX_FP_TASK, 20000, (FUNCPTR)t_dbg, 100, 100, 50, 50, 0, 0, 0, 0, 0, 0);
        udp_server();
}
