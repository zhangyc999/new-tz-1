typedef struct {
        NODE node;
        CAN *can;
} PCAN;

typedef struct {
        NODE node;
        CMD *cmd;
} PCMD;

void t_tls(void)
{
        int period = sysClkRateGet() / 4;
        int delay = period;
        int stamp;
        int recv[2];
        int send[2] = {taskIdSelf(), 0};
        int i;
        void *p;
        PCAN can[4];
        PCMD cmd[3];
        PCAN *pcan;
        PCMD *pcmd;
        LIST lstcan;
        LIST lstcmd;
        lstInit(&lstcan);
        lstInit(&lstcmd);
        for (i = 0; i < 8; i++)
                lstAdd(&lstcan, (NODE *)&can[i]);
        for (i = 0; i < 3; i++)
                lstAdd(&lstcmd, (NODE *)&cmd[i]);
        lstFirst(&lstcan)->previous = lstLast(&lstcan);
        lstFirst(&lstcmd)->previous = lstLast(&lstcmd);
        lstLast(&lstcan)->next = lstFirst(&lstcan);
        lstLast(&lstcmd)->next = lstFirst(&lstcmd);
        pcan = (PCAN *)lstFirst(&lstcan);
        pcmd = (PCMD *)lstFirst(&lstcmd);
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
