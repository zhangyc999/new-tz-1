# 全局变量定义
| 序号 | 变量名 | 说明 |
| :-: | :-: | :- |
| 1 | tid_core | 主任务ID |
| 2 | tid_udpr | UDP接收任务ID |
| 3 | tid_udpt | UDP发送任务ID |
| 4 | tid_can | CAN通信任务ID |
| 5 | tid_tls | 倾角传感器任务ID |
| 6 | tid_vsl | 视觉定位任务ID |
| 7 | tid_psu | 供电单元任务ID |
| 8 | tid_swh | 横展任务ID |
| 9 | tid_rse | 顶升任务ID |
| 10 | tid_swv | 俯仰任务ID |
| 11 | tid_prp | 垂直任务ID |
| 12 | tid_xyz | 三轴转载机构任务ID |
| 13 | tid_shd | 防护棚任务ID |
| 14 | tid_mom | 恒力矩任务ID |
| 15 | tid_dbg | 调试任务ID |
| 16 | sfd_udp | UDP套接字文件描述符 |
| 17 | port_server | 服务器（主控）端口 |
| 18 | port_client | 客户端（手持）端口 |
| 19 | addr_server | 服务器（主控）地址 |
| 20 | addr_group | 组播地址 |
| 21 | msg_core | 主任务消息队列ID |
| 22 | msg_can | CAN通信消息队列ID |
| 23 | msg_tls | 倾角传感器消息队列ID |
| 24 | msg_vsl | 视觉定位消息队列ID |
| 25 | msg_psu | 供电单元消息队列ID |
| 26 | msg_swh | 横展消息队列ID |
| 27 | msg_rse | 顶升消息队列ID |
| 28 | msg_swv | 俯仰消息队列ID |
| 29 | msg_prp | 垂直消息队列ID |
| 30 | msg_xyz | 三轴转载机构消息队列ID |
| 31 | msg_shd | 防护棚消息队列ID |
| 32 | msg_mom | 恒力矩消息队列ID |
| 33 | msg_gen | 发电机消息队列ID |
| 34 | sys_data | 系统当前状态信息 |
| 35 | sys_ecu | 各CAN节点索引及消息队列（CAN通信任务->相应子任务） |

# 函数定义

## void tz(void) {}

### 局部变量定义
| 序号 | 变量名 | 说明 |
| :-: | :-: | :- |
| 1 | n | sockaddr_in结构体长度 |
| 2 | server |  |
| 3 | group |  |

### 函数调用
| 序号 | 函数名 | 说明 |
| :-: | :-: | :- |
| 1 | htons() |  |
| 2 | htonl() |  |
| 3 | inet_addr() |  |
| 4 | routeAdd() |  |
| 5 | socket() |  |
| 6 | bind() |  |
| 7 | setsockopt() |  |
| 8 | msgQCreate() | 库函数：创建消息队列 |
| 9 | lstLibInit() | 库函数：双向循环链表功能初始化 |
| 10 | ecu_init() | 初始化各CAN节点索引及消息队列 |
| 11 | sysClkRateSet() | 库函数：设置系统调用时间片 |
| 12 | taskSpawn() | 库函数：创建任务 |

## static void ecu_init(void) {}