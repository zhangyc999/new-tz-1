#全局变量
| 序号 | 变量名 | 说明 |
| :-: | :-: | :- |
| 1 | p_can | CAN通信数据链表游标 |

# 函数定义

## void t_can(int period, int duration) {}

### 局部变量定义
| 序号 | 变量名 | 说明 |
| :-: | :-: | :- |
| 1 | recv | 接收任务ID和各子任务地址 |
| 2 | i | 循环变量 |
| 3 | id | 中间变量存储ID（29位ID左对齐） |
| 4 | buf | CAN总线缓存数据及存储数量 |
| 5 | lst | 定义链表头 |

###函数调用
| 序号 | 函数名 | 说明 |
| :-: | :-: | :- |
| 1 | lstInit | _库函数：初始化链表_ |
| 2 | lstAdd | _库函数：添加链表_ |
| 3 | lstFirst | _库函数：找到链表首节点地址_ |
| 4 | lstLast | _库函数：找到链表尾节点地址_ |
| 5 | init_can0 | 初始化CAN0总线 |
| 6 | init_can1 | 初始化CAN1总线 |
| 7 | taskDelay | _库函数：延时系统时钟周期_ |
| 8 | msgQReceive | _库函数：接收消息队列_ |
| 9 | lstPrevious | _库函数：旧链表_ |
| 10 | taskSuspend | _库函数：任务挂起_ |

## void isr_rx_can0(void) {}

### 局部变量定义

| 序号 | 变量名 | 说明 |
| :-: | :-: | :- |
| 1 | send | 发送任务ID和各子任务地址 |

###函数调用
| 序号 | 函数名 | 说明 |
| :-: | :-: | :- |
| 1 | tickGet | _库函数：获取内核时钟计数值_ |
| 2 | msgQSend | _库函数：发送消息队列_ |
| 3 | lstNext | _库函数：下一个链表地址_ |

## void isr_rx_can1(void) {}

## void init_can0(void) {}

###函数调用
| 序号 | 函数名 | 说明 |
| :-: | :-: | :- |
| 1 | sysIntDisablePIC | _库函数：禁止使能中断_ |
| 2 | intConnect | _库函数：挂断中断服务程序_ |
| 3 | sysIntEnablePIC | _库函数：使能中断_ |

## void init_can1(void) {}

