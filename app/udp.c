#include "can.h"
#include "canaddr.h"
#include "type.h"
#include "udp.h"
#include "vx.h"

extern MSG_Q_ID msg_core;

void t_udp_rx(int fd);
void t_udp_tx(int fd);

void udp_server(void)
{
        static struct sockaddr_in server;
        static struct ip_mreq group;
        static int size = sizeof(struct sockaddr_in);
        static int fd = socket(AF_INET, SOCK_DGRAM, 0);
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
        static int period = sysClkRateGet() / 10;
        static struct sockaddr_in server;
        static int size = sizeof(struct sockaddr_in);
        static int i;
        static UDP_RX buf[16];
        static UDP_RX *p;
        static LIST lst;
        lstInit(&lst);
        for (i = 0; i < 16; i++)
                lstAdd(&lst, (NODE *)&buf[i]);
        lstFirst(&lst)->previous = lstLast(&lst);
        lstLast(&lst)->next = lstFirst(&lst);
        p = (UDP_RX *)lstFirst(&lst);
        for (;;) {
                taskDelay(period);
                while (ERROR != recvfrom(fd, (str)&p->head, sizeof(*p) - sizeof(NODE) - 4, 0, (struct sockaddr *)&server, &size)) {
                        if (p->head == 0x0000FEC1) {
                                p->ts = tickGet();
                                p = (UDP_RX *)lstNext((NODE *)p);
                        }
                }
                p = (UDP_RX *)lstPrevious((NODE *)p);
                if (tickGet() - p->ts >= 0 && tickGet() - p->ts < period) {
                        if (p->cmd[0] == ((UDP_RX *)lstPrevious((NODE *)p))->cmd[0] &&
                            p->cmd[1] == ((UDP_RX *)lstPrevious((NODE *)p))->cmd[1]) {
                                msgQSend(msg_core, (str)p, sizeof(*p), NO_WAIT, MSG_PRI_NORMAL);
                        }
                }
        }
}

void t_udp_tx(int fd)
{
        static int period = sysClkRateGet() / 10;
        static struct sockaddr_in client;
        static int size = sizeof(struct sockaddr_in);
        static int index;
        static CAN buf;
        static UDP_TX tx;
        client.sin_len = (u_char)size;
        client.sin_family = AF_INET;
        client.sin_port = htons(CLIENT_PORT);
        client.sin_addr.s_addr = inet_addr(GROUP_ADDRESS);
        tx.head = 0xC7FEC7FE;
        for (;;) {
                taskDelay(period);
                while (ERROR != msgQReceive(msg_udp, (str)&buf, sizeof(buf), NO_WAIT)) {
                        index = id2index(buf.id[0]);
                        if (index > 0 && index < 50)
                                memcpy((u8 *)tx.can + (index - 1) * (sizeof(CAN) - sizeof(NODE)),
                                       buf.id, sizeof(CAN) - sizeof(NODE) - 4);
                }
                memcpy(tx.flag, &flag, sizeof(flag));
                sendto(fd, (caddr_t)&tx, sizeof(tx), 0, (struct sockaddr *)&client, size);
        }
}
