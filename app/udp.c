#include "cmd.h"
#include "data.h"
#include "vx.h"

extern MSG_Q_ID msg_core;
extern struct data sys_data;

void t_udpr(int sfd, int period, int duration)
{
        struct {
                int tid;
                struct cmd *p;
        } send;
        int size = sizeof(struct sockaddr_in);
        int len = sizeof(struct cmd) - sizeof(NODE);
        int i;
        struct sockaddr_in server;
        struct cmd buf[duration];
        struct cmd *p;
        LIST lst;
        lstInit(&lst);
        for (i = 0; i < duration; i++)
                lstAdd(&lst, (NODE *)&buf[i]);
        lstFirst(&lst)->previous = lstLast(&lst);
        lstLast(&lst)->next = lstFirst(&lst);
        p = (struct cmd *)lstFirst(&lst);
        send.tid = taskIdSelf();
        for (;;) {
                taskDelay(period);
                if (len != recvfrom(sfd, (char *)&p->head, len, MSG_PEEK, (struct sockaddr *)&server, &size))
                        continue;
                while (len == recvfrom(sfd, (char *)&p->head, len, 0, (struct sockaddr *)&server, &size))
                        ;
                if (p->head == 0xeeee && p->len == len) {
                        p->ts = tickGet();
                        send.p = p;
                        msgQSend(msg_core, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        p = (struct cmd *)lstNext((NODE *)p);
                }
        }
}

void t_udpt(int sfd, int portclient, int addrgroup, int period)
{
        int size = sizeof(struct sockaddr_in);
        struct sockaddr_in client;
        client.sin_len = (u_char)size;
        client.sin_family = AF_INET;
        client.sin_port = htons(portclient);
        client.sin_addr.s_addr = inet_addr((char *)addrgroup);
        sys_data.head = 0xcccc;
        sys_data.len = sizeof(struct data);
        for (;;) {
                taskDelay(period);
                sys_data.ts = tickGet();
                sendto(sfd, (caddr_t)&sys_data, sizeof(struct data), 0, (struct sockaddr *)&client, size);
        }
}
