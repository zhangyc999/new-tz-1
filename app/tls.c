typedef struct {
        NODE node;
        CAN *can;
} PCAN;

typedef struct {
        NODE node;
        CMD *cmd;
} PCMD;

void t_tls(int period)
{
        int delay = period;
        int stamp;
        int recv[2];
        int send[2] = {taskIdSelf(), 0};
        int ctr[2] = {0, 0};
        int i;
        void *p;
        u8 tls;
        PCAN can[2][4];
        PCAN *pcan[2];
        LIST lstcan[2];
        memset(can, 0, sizeof(can));
        lstInit(&lstcan[2]);
        for (i = 0; i < 4; i++) {
                lstAdd(&lstcan[0], (NODE *)&can[0][i]);
                lstAdd(&lstcan[1], (NODE *)&can[1][i]);
        }
        lstFirst(&lstcan[0])->previous = lstLast(&lstcan[0]);
        lstFirst(&lstcan[1])->previous = lstLast(&lstcan[1]);
        lstLast(&lstcan[0])->next = lstFirst(&lstcan[0]);
        lstLast(&lstcan[1])->next = lstFirst(&lstcan[1]);
        pcan[0] = (PCAN *)lstFirst(&lstcan[0]);
        pcan[1] = (PCAN *)lstFirst(&lstcan[1]);
        for (;;) {
                stamp = tickGet();
                if (delay < 0 || delay > period)
                        delay = 0;
                if (8 == msgQReceive(msg_tls, (str)recv, 8, delay)) {
                        switch (recv[0]) {
                        case tid_can:
                                switch (((CAN *)recv[1])->id[0]) {
                                case CA_TLS0:
                                        tls = 0;
                                        break;
                                case CA_TLS1:
                                        tls = 1;
                                        break;
                                default:
                                        break;
                                }
                                if (!pcan[tls]->can) {
                                        ctrlink[tls] = 4;
                                } else {
                                        if (tickGet() - pcan[tls]->can->ts < 50) {
                                                if (ctrlink[tls])
                                                        ctrlink[tls]--;
                                        } else {
                                                if (ctrlink[tls] < 4)
                                                        ctrlink[tls]++;
                                        }
                                }
                                switch (ctrlink[tls]) {
                                case 4:
                                        sys_data.tls[tls].fault.link = 0;
                                        break;
                                case 3:
                                        sys_data.tls[tls].fault.link = 1;
                                        break;
                                case 2:
                                        sys_data.tls[tls].fault.link = 2;
                                        break;
                                case 1:
                                        break;
                                case 0:
                                        break;
                                default:
                                        break;
                                }
                                if (pcan[tls]->can) {
                                        xsum -= *(s16 *)(pcan[tls]->can->data);
                                        ysum -= *(s16 *)(pcan[tls]->can->data + 2);
                                }
                                delay -= tickGet() - stamp;
                                break;
                        case tid_core:
                                delay -= tickGet() - stamp;
                                break;
                        default:
                                delay = period;
                                taskDelay(period);
                                break;
                        }
                }
        }
}
