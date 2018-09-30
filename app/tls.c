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
        int i;
        void *p;
        u8 tls0 = 0;
        u8 tls1 = 1;
        PCAN can[2][4];
        PCAN *pcan[2];
        LIST lstcan[2];
        lstInit(&lstcan[2]);
        for (i = 0; i < 4; i++)
                lstAdd(&lstcan, (NODE *)&can[i]);
        lstFirst(&lstcan)->previous = lstLast(&lstcan);
        lstLast(&lstcan)->next = lstFirst(&lstcan);
        pcan = (PCAN *)lstFirst(&lstcan);
        for (;;) {
                stamp = tickGet();
                if (delay < 0 || delay > period)
                        delay = 0;
                if (8 == msgQReceive(msg_tls, (str)recv, 8, delay)) {
                        switch (recv[0]) {
                        case tid_can:
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
