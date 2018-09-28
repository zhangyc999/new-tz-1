#include "can.h"
#include "canaddr.h"
#include "cmd.h"
#include "data.h"
#include "type.h"
#include "vx.h"

#define SERVER_ADDRESS "192.168.100.130"
#define GROUP_ADDRESS "234.1.1.9"
#define SERVER_PORT 4207
#define CLIENT_PORT 4201

extern MSG_Q_ID msg_core;

extern DATA sys_data;

void t_udp_rx(int fd);
void t_udp_tx(int fd);

void udp_server(void)
{
        struct sockaddr_in server;
        struct ip_mreq group;
        int n = sizeof(struct sockaddr_in);
        int fd = socket(AF_INET, SOCK_DGRAM, 0);
        /* u_long mode = 1; */
        server.sin_len = (u_char)n;
        server.sin_family = AF_INET;
        server.sin_port = htons(SERVER_PORT);
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        /* ioctl(fd, FIONBIO, (int)&mode); */
        bind(fd, (struct sockaddr *)&server, n);
        group.imr_multiaddr.s_addr = inet_addr(GROUP_ADDRESS);
        group.imr_interface.s_addr = inet_addr(SERVER_ADDRESS);
        routeAdd(GROUP_ADDRESS, SERVER_ADDRESS);
        setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (str)&group, sizeof(group));
        taskSpawn("UDP_RX", 90, VX_FP_TASK, 20000, (FUNCPTR)t_udp_rx, fd, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        taskSpawn("UDP_TX", 90, VX_FP_TASK, 20000, (FUNCPTR)t_udp_tx, fd, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void t_udp_rx(int fd)
{
        int period = sysClkRateGet() / 10;
        struct sockaddr_in server;
        int n = sizeof(struct sockaddr_in);
        int len = sizeof(CMD) - sizeof(NODE);
        int i;
        CMD buf[64];
        CMD *p;
        LIST lst;
        lstInit(&lst);
        for (i = 0; i < 64; i++)
                lstAdd(&lst, (NODE *)&buf[i]);
        lstFirst(&lst)->previous = lstLast(&lst);
        lstLast(&lst)->next = lstFirst(&lst);
        p = (CMD *)lstFirst(&lst);
        for (;;) {
                taskDelay(period);
                if (len != recvfrom(fd, (str)&p->head, len, MSG_PEEK, (struct sockaddr *)&server, &n))
                        continue;
                while (len == recvfrom(fd, (str)&p->head, len, 0, (struct sockaddr *)&server, &n))
                        ;
                if (p->head == 0xfec1 && p->len == len) {
                        p->ts = tickGet();
                        msgQSend(msg_core, (str)&p, 4, NO_WAIT, MSG_PRI_NORMAL);
                        p = (CMD *)lstNext((NODE *)p);
                }
        }
}

void t_udp_tx(int fd)
{
        int period = sysClkRateGet() / 10;
        struct sockaddr_in client;
        int n = sizeof(struct sockaddr_in);
        client.sin_len = (u_char)n;
        client.sin_family = AF_INET;
        client.sin_port = htons(CLIENT_PORT);
        client.sin_addr.s_addr = inet_addr(GROUP_ADDRESS);
        sys_data.head = 0xc7fe;
        sys_data.len = sizeof(DATA);
        for (;;) {
                taskDelay(period);
                sys_data.ts = tickGet();
                sendto(fd, (caddr_t)&sys_data, sizeof(DATA), 0, (struct sockaddr *)&client, n);
        }
}
