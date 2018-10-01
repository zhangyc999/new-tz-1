#include "cmd.h"
#include "data.h"
#include "type.h"
#include "vx.h"

extern int sfd_udp;
extern int port_client;
extern str addr_group;
extern MSG_Q_ID msg_core;
extern DATA sys_data;

void t_udpr(int period, int duration)
{
        struct {
                int tid;
                CMD *p;
        } send;
        int n = sizeof(struct sockaddr_in);
        int len = sizeof(CMD) - sizeof(NODE);
        int i;
        struct sockaddr_in server;
        CMD buf[duration];
        CMD *p;
        LIST lst;
        lstInit(&lst);
        for (i = 0; i < duration; i++)
                lstAdd(&lst, (NODE *)&buf[i]);
        lstFirst(&lst)->previous = lstLast(&lst);
        lstLast(&lst)->next = lstFirst(&lst);
        p = (CMD *)lstFirst(&lst);
        send.tid = taskIdSelf();
        for (;;) {
                taskDelay(period);
                if (len != recvfrom(sfd_udp, (str)&p->head, len, MSG_PEEK, (struct sockaddr *)&server, &n))
                        continue;
                while (len == recvfrom(sfd_udp, (str)&p->head, len, 0, (struct sockaddr *)&server, &n))
                        ;
                if (p->head == 0xeeee && p->len == len) {
                        p->ts = tickGet();
                        send.p = p;
                        msgQSend(msg_core, (str)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                        p = (CMD *)lstNext((NODE *)p);
                }
        }
}

void t_udpt(int period)
{
        int n = sizeof(struct sockaddr_in);
        struct sockaddr_in client;
        client.sin_len = (u_char)n;
        client.sin_family = AF_INET;
        client.sin_port = htons(port_client);
        client.sin_addr.s_addr = inet_addr(addr_group);
        sys_data.head = 0xcccc;
        sys_data.len = sizeof(DATA);
        for (;;) {
                taskDelay(period);
                sys_data.ts = tickGet();
                sendto(sfd_udp, (caddr_t)&sys_data, sizeof(DATA), 0, (struct sockaddr *)&client, n);
        }
}
