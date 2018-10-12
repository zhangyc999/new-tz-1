#include "cmd.h"
#include "data.h"
#include "vx.h"

extern MSG_Q_ID msg_core;
extern struct data sys_data;

void t_udpr(int sfd, int period, int duration)
{
        int i; /* 循环变量 */
        int size = sizeof(struct sockaddr_in); /* sockaddr_in结构体长度 */
        int len = sizeof(struct cmd) - sizeof(NODE); /* 命令长度 */
        struct sockaddr_in server; /* 定义服务器端 */
        struct cmd buf[duration]; /* 以太网接收数据存储空间 */
        struct cmd *p; /* 以太网接收缓冲链表游标 */
        struct {
                int tid; /* 自身任务ID */
                struct cmd *p; /* 接收数据首地址 */
        } send;
        LIST lst; /* 定义链表头 */
        /* 开始：构建接收缓冲链表 */
        lstInit(&lst);
        for (i = 0; i < duration; i++)
                lstAdd(&lst, (NODE *)&buf[i]);
        lstFirst(&lst)->previous = lstLast(&lst);
        lstLast(&lst)->next = lstFirst(&lst);
        /* 结束：构建接收缓冲链表 */
        p = (struct cmd *)lstFirst(&lst);
        send.tid = taskIdSelf();
        for (;;) {
                taskDelay(period); /* 延时 */
                if (len != recvfrom(sfd, (char *)&p->head, len, 0, (struct sockaddr *)&server, &size)) /* 套接字缓冲为空 */
                        continue;
                while (len == recvfrom(sfd, (char *)&p->head, len, 0, (struct sockaddr *)&server, &size)) /* 获取套接字缓冲最新数据并拷贝到缓冲链表中 */
                        ;
                if (p->head == 0xeeee && p->len == len) { /* 帧头和帧长度都正确 */
                        p->ts = tickGet(); /* 打上时标 */
                        send.p = p;
                        msgQSend(msg_core, (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL); /* 发送消息队列到主任务 */
                        p = (struct cmd *)lstNext((NODE *)p); /* 游标指向下一个节点 */
                }
        }
}

void t_udpt(int sfd, int portclient, int addrgroup, int period)
{
        int size = sizeof(struct sockaddr_in); /* sockaddr_in结构体长度 */
        struct sockaddr_in client; /* 定义客户端 */
        /* 开始：配置客户端 */
        client.sin_len = (u_char)size;
        client.sin_family = AF_INET;
        client.sin_port = htons(portclient);
        client.sin_addr.s_addr = inet_addr((char *)addrgroup);
        /* 结束：配置客户端 */
        sys_data.head = 0xcccc; /* 设置固定头指针 */
        sys_data.len = sizeof(struct data); /* 设置数据长度 */
        for (;;) {
                taskDelay(period); /* 延时 */
                sys_data.ts = tickGet(); /* 打上时标 */
                sendto(sfd, (caddr_t)&sys_data, sizeof(struct data), 0, (struct sockaddr *)&client, size); /* 发送数据到客户端 */
        }
}
