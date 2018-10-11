#include "vx.h"

static int id2index(unsigned char id);

void t_rse(void)
{
        static int period = sysClkRateGet() / 10;
        static int delay = period;
        static int tmp[2];
        static int i;
        static int j;
        static int index;
        static int lock;
        static unsigned char tmp[sizeof(struct ext)];
        static struct ext buf[4][16];
        static struct ext *tmp;
        static struct ext *p[4];
        static UDP_RX udp;
        static LIST lst[4];
        for (i = 0; i < 4; i++) {
                lstInit(&lst[i]);
                for (j = 0; j < 16; j++)
                        lstAdd(&lst[i], (NODE *)&buf[i][j]);
                lstFirst(&lst[i])->previous = lstLast(&lst[i]);
                lstLast(&lst[i])->next = lstFirst(&lst[i]);
                p[i] = (struct ext *)lstFirst(&lst[i]);
        }
        taskSuspend(0);
        for (;;) {
                prev = tickGet();
                if (delay < 0 || delay > period)
                        delay = 0;
                len = msgQReceive(msg_rse, (char *)tmp, sizeof(tmp), delay);
                switch (len) {
                case sizeof(struct ext):
                        if (tmp->id[2] == 0xC6) {
                                index = id2index(tmp->id[0]);
                                if (index > 0 && index < 5) {
                                        index -= 1;
                                        *p = *tmp;
                                        sum_pos[index] -= (int)(*(short *)(&tmp->data[0])) * 20;
                                        sum_vel[index] -= (int)(*(short *)(&tmp->data[2]));
                                        sum_ampr[index] -= (int)(*(short *)(&tmp->data[4]));
                                        cur_pos[index] = (int)(*(short *)(&p->data[0])) * 20;
                                        cur_vel[index] = (int)(*(short *)(&p->data[2]));
                                        cur_ampr[index] = (int)(*(short *)(&p->data[4]));
                                        cur_fault[index] = (int)(*(unsigned char *)(&p->data[6]));
                                        cur_io[index] = (int)(*(unsigned char *)(&p->data[7]));
                                        sum_pos[index] += cur_pos[index];
                                        sum_vel[index] += cur_vel[index];
                                        sum_ampr[index] += cur_ampr[index];
                                        if (period <= period_fast) {
                                                len_lst = 3;
                                                period_ref = period_fast;
                                        } else {
                                                len_lst = 1;
                                                period_ref = period_slow;
                                        }
                                        while (lstCount(&lst[index]) > len_lst) {
                                                tmp = (struct ext *)lstGet(&lst[index]);
                                                sum_pos[index] -= (int)(*(short *)(&tmp->data[0])) * 20;
                                                sum_vel[index] -= (int)(*(short *)(&tmp->data[2]));
                                                sum_ampr[index] -= (int)(*(short *)(&tmp->data[4]));
                                                free(tmp);
                                        }
                                        avg_pos[index] = sum_pos[index] / lstCount(&lst[index]);
                                        avg_vel[index] = sum_vel[index] / lstCount(&lst[index]);
                                        cur_ampr[index] = sum_ampr[index] / lstCount(&lst[index]);
                                        pos_avg = (cur_pos[0] + cur_pos[1] + cur_pos[2] + cur_pos[3]) / 4;
                                        vel_avg = (cur_vel[0] + cur_vel[1] + cur_vel[2] + cur_vel[3]) / 4;
                                        ampr_avg = (cur_ampr[0] + cur_ampr[1] + cur_ampr[2] + cur_ampr[3]) / 4;
                                        pos_max = max(cur_pos[0], max(cur_pos[1], max(cur_pos[2], cur_pos[3])));
                                        pos_min = min(cur_pos[0], min(cur_pos[1], min(cur_pos[2], cur_pos[3])));
                                        vel_max = max(cur_vel[0], max(cur_vel[1], max(cur_vel[2], cur_vel[3])));
                                        vel_min = min(cur_vel[0], min(cur_vel[1], min(cur_vel[2], cur_vel[3])));
                                        ampr_max = max(cur_ampr[0], max(cur_ampr[1], max(cur_ampr[2], cur_ampr[3])));
                                        ampr_min = min(cur_ampr[0], min(cur_ampr[1], min(cur_ampr[2], cur_ampr[3])));
                                        if (lstCount(&lst[index]) == len_lst) {
                                                delta = tickGet() - ((struct ext *)lstFirst(&lst[index]))->ts;
                                                if (delta < (len_lst + 1) * period_ref) {
                                                        lock = taskLock();
                                                        flag.rse[index] &= ~FLAG_FAULT_COMM;
                                                        taskUnlock(lock);
                                                } else if (delta > (len_lst + 2) * period_ref) {
                                                        lock = taskLock();
                                                        flag.rse[index] |= FLAG_FAULT_COMM;
                                                        taskUnlock(lock);
                                                }
                                        } else {
                                                lock = taskLock();
                                                flag.rse[index] |= FLAG_FAULT_COMM;
                                                taskUnlock(lock);
                                        }
                                        if (avg_pos[index] < min_pos[index] - 2000 || avg_pos[index] > max_pos[index] + 2000)
                                                flag_rse[index] |= FLAG_FAULT_POS;
                                        else if (avg_pos[index] > min_pos[index] - 1500 && avg_pos[index] < max_pos[index] + 1500)
                                                flag_rse[index] &= ~FLAG_FAULT_POS;
                                        if (avg_vel[index] < min_vel[index] - 200 || avg_vel[index] > max_vel[index] + 200)
                                                flag_rse[index] |= FLAG_FAULT_VEL;
                                        else if (avg_vel[index] > min_vel[index] - 100 && avg_vel[index] < max_vel[index] + 100)
                                                flag_rse[index] &= ~FLAG_FAULT_VEL;
                                        if (avg_ampr[index] < min_ampr[index] - 100 || avg_ampr[index] > max_ampr[index] + 100)
                                                flag_rse[index] |= FLAG_FAULT_AMPR;
                                        else if (avg_ampr[index] > min_ampr[index] - 50 && avg_ampr[index] < max_ampr[index] + 50)
                                                flag_rse[index] &= ~FLAG_FAULT_AMPR;
                                        if (avg_pos[index] < min_pos[index] - 100)
                                                flag.rse[index] |= FLAG_START;
                                        else if (avg_pos[index] > min_pos[index] + 100)
                                                flag.rse[index] &= ~FLAG_START;
                                        if (avg_pos[index] > max_pos[index] + 100)
                                                flag.rse[index] |= FLAG_FINAL;
                                        else if (avg_pos[index] < max_pos[index] - 100)
                                                flag.rse[index] &= ~FLAG_FINAL;
                                        if (avg_pos[index] < lock_pos[index] - 100)
                                                flag.rse[index] |= FLAG_RSE_LOCK;
                                        else if (avg_pos[index] > lock_pos[index] + 100)
                                                flag.rse[index] &= ~FLAG_RSE_LOCK;
                                        if (avg_pos[index] < leave_pos[index] - 100)
                                                flag.rse[index] |= FLAG_RSE_LEAVE;
                                        else if (avg_pos[index] > leave_pos[index] + 100)
                                                flag.rse[index] &= ~FLAG_RSE_LEAVE;
                                        if (avg_pos[index] > joint_pos[index] + 100)
                                                flag.rse[index] |= FLAG_RSE_JOINT;
                                        else if (avg_pos[index] < joint_pos[index] - 100)
                                                flag.rse[index] &= ~FLAG_RSE_JOINT;
                                        if (avg_vel[index] > -3 && avg_vel[index] < 3)
                                                flag.rse[index] |= FLAG_STOP;
                                        else if (avg_vel[index] < -10 || avg_vel[index] > 10)
                                                flag.rse[index] &= ~FLAG_STOP;
                                        if (FLAG_STOP & flag.rse[0] & flag.rse[1] & flag.rse[2] & flag.rse[3]) {
                                                if (pos_max - pos_min > err_sync * 2 + 100) {
                                                        for (i = 0; i < 4; i++)
                                                                flag.rse[index] |= FLAG_FAULT_SYNC;
                                                } else if (pos_max - pos_min < err_sync * 2 - 100) {
                                                        for (i = 0; i < 4; i++)
                                                                flag.rse[index] &= ~FLAG_FAULT_SYNC;
                                                }
                                        } else {
                                                if (pos_max - pos_min > err_sync + 100) {
                                                        for (i = 0; i < 4; i++)
                                                                flag.rse[index] |= FLAG_FAULT_SYNC;
                                                } else if (pos_max - pos_min < err_sync - 100) {
                                                        for (i = 0; i < 4; i++)
                                                                flag.rse[index] &= ~FLAG_FAULT_SYNC;
                                                }
                                        }
                                        tmp = (struct ext *)lstLast(&lst[index]);
                                        ctr_fault[index] = 0;
                                        do {
                                                if (cur_fault[index] == tmp->data[6])
                                                        ctr_fault[index]++;
                                                else
                                                        ctr_fault[index] = 0;
                                        } while (tmp = (struct ext *)lstPrevious((NODE *)tmp));
                                        if (ctr_fault[index] == len_lst) {
                                                switch (cur_fault[index]) {
                                                case 0x00:
                                                case 0x03:
                                                        flag.rse[index] &= ~FLAG_FAULT_GENERAL;
                                                        flag.rse[index] &= ~FLAG_FAULT_SERIOUS;
                                                        break;
                                                case 0x0C:
                                                        flag.rse[index] |= FLAG_FAULT_GENERAL;
                                                        break;
                                                case 0xF0:
                                                        flag.rse[index] |= FLAG_FAULT_SERIOUS;
                                                        break;
                                                default:
                                                        break;
                                                }
                                        }
                                        tmp = (struct ext *)lstLast(&lst[index]);
                                        ctr_io[index] = 0;
                                        do {
                                                if (cur_io[index] == tmp->data[7])
                                                        ctr_io[index]++;
                                                else
                                                        ctr_io[index] = 0;
                                        } while (tmp = (struct ext *)lstPrevious((NODE *)tmp));
                                        if (ctr_io[index] == len_lst)
                                                flag.rse[index] = flag.rse[index] & ~FLAG_IO | cur_io[index];
                                }
                        }
                        delay -= tickGet() - prev;
                        break;
                case sizeof(UDP_RX):
                        udp = *(UDP_RX *)tmp;
                        switch (udp.cmd[0] & UMASK_UDP_MODE) {
                        case UDP_MODE_STUPID:
                                break;
                        case UDP_MODE_MANUAL:
                                break;
                        case UDP_MODE_EXPERT:
                                break;
                        default:
                                break;
                        }
                        delay -= tickGet() - prev;
                        break;
                default:
                        delay = period;
                        break;
                }
        }
}

static int id2index(unsigned char id)
{
        static int index[255];
        index[CA_RSE0] = 1;
        index[CA_RSE1] = 2;
        index[CA_RSE2] = 3;
        index[CA_RSE3] = 4;
        return index[id];
}
