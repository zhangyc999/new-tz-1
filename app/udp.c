#include "can.h"
#include "canaddr.h"
#include "data.h"
#include "type.h"
#include "udp.h"
#include "vx.h"

extern MSG_Q_ID msg_core;

extern DATA sys_data;

void t_udp_rx(int fd);
void t_udp_tx(int fd);

void udp_server(void)
{
        struct sockaddr_in server;
        struct ip_mreq group;
        int size = sizeof(struct sockaddr_in);
        int fd = socket(AF_INET, SOCK_DGRAM, 0);
        /* u_long mode = 1; */
        server.sin_len = (u_char)size;
        server.sin_family = AF_INET;
        server.sin_port = htons(SERVER_PORT);
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        /* ioctl(fd, FIONBIO, (int)&mode); */
        bind(fd, (struct sockaddr *)&server, size);
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
        int size = sizeof(struct sockaddr_in);
        int i;
        UDP buf[16];
        UDP *p;
        LIST lst;
        int len = sizeof(*p) - sizeof(NODE) - 4;
        lstInit(&lst);
        for (i = 0; i < 16; i++)
                lstAdd(&lst, (NODE *)&buf[i]);
        lstFirst(&lst)->previous = lstLast(&lst);
        lstLast(&lst)->next = lstFirst(&lst);
        p = (UDP *)lstFirst(&lst);
        for (;;) {
                taskDelay(period);
                while (len == recvfrom(fd, (str)&p->head, len, 0, (struct sockaddr *)&server, &size)) {
                        if (p->head == 0xfec1 && p->len == len) {
                                p->ts = tickGet();
                                p = (UDP *)lstNext((NODE *)p);
                        }
                }
                p = (UDP *)lstPrevious((NODE *)p);
                if (tickGet() - p->ts >= 0 && tickGet() - p->ts < period) {
                        if (p->cmd.src == ((UDP *)lstPrevious((NODE *)p))->cmd.src &&
                            p->cmd.dev == ((UDP *)lstPrevious((NODE *)p))->cmd.dev &&
                            p->cmd.mode == ((UDP *)lstPrevious((NODE *)p))->cmd.mode &&
                            p->cmd.act == ((UDP *)lstPrevious((NODE *)p))->cmd.act)
                                msgQSend(msg_core, (str)&p->cmd, sizeof(p->cmd), NO_WAIT, MSG_PRI_NORMAL);
                }
        }
}

void t_udp_tx(int fd)
{
        int period = sysClkRateGet() / 10;
        struct sockaddr_in client;
        int size = sizeof(struct sockaddr_in);
        client.sin_len = (u_char)size;
        client.sin_family = AF_INET;
        client.sin_port = htons(CLIENT_PORT);
        client.sin_addr.s_addr = inet_addr(GROUP_ADDRESS);
        sys_data.head = 0xC7FE;
        sys_data.len = 800;
        for (;;) {
                taskDelay(period);
                sys_data.ts = tickGet();
                sendto(fd, (caddr_t)&sys_data, sizeof(sys_data), 0, (struct sockaddr *)&client, size);
        }
}
