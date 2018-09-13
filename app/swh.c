#include "type.h"
#include "vx.h"

static int id2index(u8 id);

void t_swh(int period_slow, int period_fast, int prio_low, int prio_high)
{
        int tid = taskIdSelf();
        int prio = prio_low;
        int period = period_slow;
        int tmp[2] = {0};
        int i = 0;
        int index = 0;
        int lock;
        u8 tmp[sizeof(CAN)];
        CAN *can;
        CAN *tmp;
        UDP_RX udp;
        LIST lst[4];
        for (i = 0; i < 4; i++)
                lstInit(&lst[i]);
        taskSuspend(0);
        for (;;) {
                prev = tickGet();
                if (period < 0 || period > period_slow)
                        period = 0;
                len = msgQReceive(msg_swh, (str)tmp, sizeof(tmp), period);
                switch (len) {
                case sizeof(CAN):
                        can = (CAN *)tmp;
                        if (can->id[2] == 0xC6) {
                                index = id2index(can->id[0]);
                                if (index > 0 && index < 5) {
                                        i = index - 1;
                                        can = malloc(sizeof(*can));
                                        *can = *(CAN *)tmp;
                                        cur_pos[i] = (int)(*(s16 *)(&can->data[0])) * 20;
                                        cur_vel[i] = (int)(*(s16 *)(&can->data[2]));
                                        cur_ampr[i] = (int)(*(s16 *)(&can->data[4]));
                                        sum_pos[i] += cur_pos[i];
                                        sum_vel[i] += cur_vel[i];
                                        sum_ampr[i] += cur_ampr[i];
                                        lstAdd(&lst[i], (NODE *)can);
                                        if (period <= period_fast) {
                                                while (lstCount(&lst[i]) > 3) {
                                                        tmp = (CAN *)lstGet(&lst[i]);
                                                        sum_pos[i] -= (int)(*(s16 *)(&tmp->data[0])) * 20;
                                                        sum_vel[i] -= (int)(*(s16 *)(&tmp->data[2]));
                                                        sum_ampr[i] -= (int)(*(s16 *)(&tmp->data[4]));
                                                        free(tmp);
                                                }
                                                if (lstCount(&lst[i]) == 3 &&
                                                    tickGet() - ((CAN *)lstFirst(&lst[i]))->ts < 4 * period_fast) {
                                                        lock = taskLock();
                                                        flag.swh[i] &= ~FLAG_FAULT_COMM;
                                                        taskUnlock(lock);
                                                } else {
                                                        lock = taskLock();
                                                        flag.swh[i] |= FLAG_FAULT_COMM;
                                                        taskUnlock(lock);
                                                }
                                        } else {
                                                while (lstCount(&lst[i]) > 1) {
                                                        tmp = (CAN *)lstGet(&lst[i]);
                                                        sum_pos[i] -= (int)(*(s16 *)(&tmp->data[0])) * 20;
                                                        sum_vel[i] -= (int)(*(s16 *)(&tmp->data[2]));
                                                        sum_ampr[i] -= (int)(*(s16 *)(&tmp->data[4]));
                                                        free(tmp);
                                                }
                                                if (lstCount(&lst[i]) == 1 &&
                                                    tickGet() - ((CAN *)lstFirst(&lst[i]))->ts < 2 * period_slow) {
                                                        lock = taskLock();
                                                        flag.swh[i] &= ~FLAG_FAULT_COMM;
                                                        taskUnlock(lock);
                                                } else {
                                                        lock = taskLock();
                                                        flag.swh[i] |= FLAG_FAULT_COMM;
                                                        taskUnlock(lock);
                                                }
                                        }
                                        avg_pos[i] = sum_pos[i] / lstCount(&lst[i]);
                                        avg_vel[i] = sum_vel[i] / lstCount(&lst[i]);
                                        avg_ampr[i] = sum_ampr[i] / lstCount(&lst[i]);
                                }
                        }
                        period -= tickGet() - prev;
                        break;
                case sizeof(UDP_RX):
                        udp = *(UDP_RX *)tmp;
                        period -= tickGet() - prev;
                        break;
                default:
                        period = period_slow;
                        break;
                }
        }
}

static int id2index(u8 id)
{
        static int index[255] = {0};
        index[CA_SWH0] = 1;
        index[CA_SWH1] = 2;
        index[CA_SWH2] = 3;
        index[CA_SWH3] = 4;
        return index[id];
}
