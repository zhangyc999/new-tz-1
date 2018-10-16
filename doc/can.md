# t_can()

```c
void                 /* 无返回值 */
t_can(               /* CAN通信任务 */
        int addr,    /* CAN总线基地址表 */
        int irq,     /* CAN总线中断号表 */
        int n,       /* CAN总线数量 */
        int period,  /* 任务运行周期 */
        int duration /* CAN总线接收缓冲链表长度 */
)
```

```mermaid
graph TB
        st((开始))-->构建缓冲链表
        构建缓冲链表-->硬件初始化
        硬件初始化-->loop(主循环开始)
        loop-->延时
        延时-->a{状态寄存器正常}
        a--YES-->b{消息队列为空}
        a--NO-->重新初始化硬件
        重新初始化硬件-->loop
        b--YES-->loop
        b--NO-->向总线发送
        向总线发送-->c{总线待发送数据数量过多}
        c--YES-->d[总线负载率过高]
        c--NO-->e{总线待发送数据数量警告}
        d-->loop
        e--YES-->f{缓冲链表已满}
        e--NO-->g{总线待发送数据数量正常}
        f--YES-->h{缓冲累积时间过小}
        f--NO-->总线负载率警告
        总线负载率警告-->loop
        g--YES-->i{缓冲链表已满}
        g--NO-->总线负载率正常
        h--YES-->j[总线负载率过高]
        h--NO-->总线负载率警告
        i--YES-->k{缓冲积累时间过小}
        i--NO-->总线负载率正常
        j-->loop
        k--YES-->l[总线负载率过高]
        k--NO-->总线负载率正常
        l-->loop
        总线负载率正常-->loop
```
# isr_rx_can0()
# isr_rx_can1()

```c
static void      /* 无返回值 */
isr_rx_can0(     /* CAN总线0接收中断服务程序 */
        int addr /* CAN总线0基地址 */
);

static void      /* 无返回值 */
isr_rx_can1(     /* CAN总线1接收中断服务程序 */
        int addr /* CAN总线1基地址 */
);
```

```mermaid
graph TB
        st((开始))-->a{中断和状态寄存器正常}
        a--YES-->b{是扩展数据帧}
        a--NO-->释放接收缓冲器
        b--YES-->接收数据并拷贝到缓冲链表中
        b--NO-->释放接收缓冲器
        接收数据并拷贝到缓冲链表中-->c{收到的ID和数据正确}
        c--YES-->加入时标
        c--NO-->释放接收缓冲器
        加入时标-->发送到相应的消息队列
        发送到相应的消息队列-->游标指向下一个节点
        游标指向下一个节点-->释放接收缓冲器
        释放接收缓冲器-->en((结束))
```

# init_can()

```c
static void        /* 无返回值 */
init_can(
        int *addr, /* CAN总线基地址表 */
        int *irq,  /* CAN总线中断号表 */
        int cable  /* 总线编号 */
)
```
