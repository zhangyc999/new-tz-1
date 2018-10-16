# t_udpr()

```c
void                 /* 无返回值 */
t_udpr(              /* UDP接收任务 */
        int sfd,     /* 套接字 */
        int period,  /* 任务运行周期 */
        int duration /* 以太网接收缓冲链表长度 */
)
```

```mermaid
graph TB
        st((开始))-->构建缓冲链表
        构建缓冲链表-->loop(主循环开始)
        loop-->延时
        延时-->a{套接字数据为空}
        a--YES-->loop
        a--NO-->获取套接字缓冲最新数据并拷贝到缓冲链表中
        获取套接字缓冲最新数据并拷贝到缓冲链表中-->b{帧头和帧长度都正确}
        b--YES-->打上时标
        b--NO-->loop
        打上时标-->发送消息队列到主任务
        发送消息队列到主任务-->游标指向下一个节点
        游标指向下一个节点-->loop


```

# t_udpt()

```c
void                    /* 无返回值 */
t_udpt(                 /* UDP发送任务 */
        int sfd,        /* 套接字 */
        int portclient, /* 客户端接口 */
        int addrgroup,  /* 组播地址 */
        int period      /* 任务运行周期 */
)
```

```mermaid
graph TB
        st((开始))-->配置客户端
        配置客户端-->设置固定帧头
        设置固定帧头-->设置数据长度
        设置数据长度-->loop(主循环开始)
        loop-->延时
        延时-->打上时标
        打上时标-->发送数据到客户端
        发送数据到客户端-->loop
```
