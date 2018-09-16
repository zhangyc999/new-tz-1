#include "can.h"
#include "canaddr.h"
#include "flag.h"
#include "type.h"
#include "udp.h"
#include "vx.h"

extern void t_udp_rx(int fd);
extern void t_udp_tx(int fd);

extern int tid_can;

extern MSG_Q_ID msg_core;
extern MSG_Q_ID msg_udp;

extern SYS_FLAG flag;

int tid_udp_rx;
int tid_udp_tx;

static int id2index(u8 id);

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
        setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group));
        tid_udp_rx = taskSpawn("UDP_RX", 90, VX_FP_TASK, 20000, (FUNCPTR)t_udp_rx, fd, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        tid_udp_tx = taskSpawn("UDP_TX", 90, VX_FP_TASK, 20000, (FUNCPTR)t_udp_tx, fd, 0, 0, 0, 0, 0, 0, 0, 0, 0);
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

static int id2index(u8 id)
{
        static int index[255];
        index[CA_TLS0] = 3;
        index[CA_TLS1] = 4;
        index[CA_VSL0] = 1;
        index[CA_VSL1] = 2;
        index[CA_PSU] = 31;
        index[CA_MOM0] = 33;
        index[CA_MOM1] = 34;
        index[CA_MOM2] = 35;
        index[CA_MOM3] = 36;
        index[CA_SWH0] = 25;
        index[CA_SWH1] = 26;
        index[CA_SWH2] = 27;
        index[CA_SWH3] = 28;
        index[CA_RSE0] = 17;
        index[CA_RSE1] = 18;
        index[CA_RSE2] = 19;
        index[CA_RSE3] = 20;
        index[CA_SWV0] = 21;
        index[CA_SWV1] = 22;
        index[CA_SWV2] = 23;
        index[CA_SWV3] = 24;
        index[CA_PRP0] = 13;
        index[CA_PRP1] = 14;
        index[CA_PRP2] = 15;
        index[CA_PRP3] = 16;
        index[CA_SDT] = 37;
        index[CA_X0] = 11;
        index[CA_X1] = 12;
        index[CA_Y0] = 7;
        index[CA_Y1] = 8;
        index[CA_Y2] = 9;
        index[CA_Y3] = 10;
        index[CA_Z0] = 5;
        index[CA_Z1] = 6;
        return index[id];
}
