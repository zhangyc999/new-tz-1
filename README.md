# new-tz-1
The Main Controller Program of Large Tonnage Transshipping Vehicle 大吨位转载车主控程序

[TOC]

## Functional Modules 功能模块

### Communication 通信
#### CAN通信
#### ETH通信(UDP)

### Power Management 电源管理
#### ON/OFF 通断电
#### Delay 延时上电

### Multi-Servo Contrl 多伺服协同运动控制
#### Single Servo Control 单伺服运动控制
#### Synchronization 同步控制
#### Automatic Levelling 自动调平
#### Visual Locating 视觉定位

### Degrade 降级使用
#### Tilt Sensor 倾角传感器
#### Visual Locator 视觉定位
#### Servo 伺服

### Safety Control 安全控制
#### Device Warning 设备报警
#### Communication 通信中断
#### Max & Min 范围限制
#### Change Rate 变化率限制
#### Process Interlock 流程互锁
#### Synchronization 同步限制
##### Position 位置同步
##### Velocity 速度同步
##### Current 电流同步
##### Triaxial Mechanism 三轴转载机构
#### Forecast & Dodge 预测规避

### Repair & Maintenance 维修维护
#### Open Expert Channel 打开专家模式ETH-CAN通道
#### Automatic Close 超时自动关闭通道

## MultiTask Design 多任务设计

### t_can CAN通信任务
#### Input 输入及其相关处理
- 各子任务控制命令
    + 利用3层队列接收（高实时性、中实时性、低实时性）
    + 总线负载情况分析
- CAN总线数据
    + 中断响应接收
#### Output 输出及其相关处理
- 子任务生成的控制命令
    + 遵循既定时隙向2条总线发送
- 向子任务发送CAN总线数据
    + 分析数据来源后发送给相应子任务
- 向UDP通信任务发送CAN总线数据
- 向调试任务发送CAN总线数据
#### INIT() 初始化
#### isr0() CAN0接收中断服务程序
#### isr1() CAN1接收中断服务程序
### t_udp_rx UDP通信接收任务
### t_udp_tx UDP通信发送任务
### t_core 主任务
#### Input 输入及其相关处理
- ETH操控指令
    + 判断合法性
    + 缓冲处理
    + 检测通信中断
#### Output 输出及其相关处理
- 根据当前操控指令完成相关设备自动上电操作
- 根据当前操控指令完成其它设备自动上电/断电操作
- 根据当前操控指令运行相应子任务
- 根据当前操控指令运行/挂起其它子任务
### t_tls 倾角传感器任务
#### Input 输入及其相关处理
- CAN总线倾角传感器数据
    + 缓冲处理
    + 检测通信中断
    + 设备报警处理
    + 双倾角传感器冗余处理
    + 倾角传感器范围判定
        * 超限（禁止所有伺服动作）
        * 一般（需要调平)
        * 良好（无须调平）
- 降级使用请求
#### Output 输出及其相关处理
- 倾角传感器范围判定结果
    + 写入系统参数标志组
### t_vsl 视觉定位任务
#### Input 输入及其相关处理
- 降级使用请求
#### Output 输出及其相关处理
### t_psu 电源管理任务
#### Input 输入及其相关处理
- 降级使用请求
#### Output 输出及其相关处理
### t_swh 横向摆臂任务
#### Input 输入及其相关处理
- 降级使用请求
#### Output 输出及其相关处理
### t_rse 顶升托举任务
#### Input 输入及其相关处理
- 降级使用请求
#### Output 输出及其相关处理
### t_swv 俯仰摆腿任务
#### Input 输入及其相关处理
- 降级使用请求
#### Output 输出及其相关处理
### t_prp 垂直支腿任务
#### Input 输入及其相关处理
- 降级使用请求
#### Output 输出及其相关处理
### t_sdt 顶防护任务
#### Input 输入及其相关处理
- 降级使用请求
#### Output 输出及其相关处理
### t_sds 侧防护任务
### t_sdfb 前后端防护任务
### t_xyz 三轴转载机构任务
#### Input 输入及其相关处理
- 降级使用请求
#### Output 输出及其相关处理
### t_dbg 调试任务
#### Input 输入及其相关处理
#### Output 输出及其相关处理

## Process Design 流程设计

### Power 电源管理
手动选择伺服驱动器、照明灯、定位灯开关

### Spread 展开

#### Swing(Horizontal) 横向摆臂
##### Tilt Sensor 倾角传感器倾角满足要求
##### Moment Machinery 恒力矩电机上电
##### Moment Machinery 恒力矩电机力矩满足要求
##### Power Legs 支腿上电

#### Raise 顶升托举

#### Swing(Vertical) 俯仰摆腿

#### Prop 垂直支腿

#### Shield(Top) 顶防护

### Transship 转载

#### Triaxial Mechanism(X) 三轴转载机构X

#### Triaxial Mechanism(Y) 三轴转载机构Y

#### Triaxial Mechanism(Z) 三轴转载机构Z

### Retreat 撤收

## Data Transfer Model 数据传输模型

## Coordinate System 坐标系

## Algorithm 算法

## Code Architecture 代码架构

### app/
- [x] **tz.c** -- *Initialization of Whole System*
- [x] **can.c** -- *Task of Communication on CAN Bus*
    - [x] **void t_can()**
    - [x] **static void isr0()**
    - [x] **static void isr1()**
    - [x] **static void init0()**
    - [x] **static void init1()**
    - [x] **static MSG_Q_ID id2msg()**
- [ ] **core.c** -- 
- [ ] **rse.c** -- *Task to Deal With Four Raise Arms*
- [x] **udp.c** -- *Tasks of Communication on Ethernet With UDP Protocol*
### include/
- [x] **can.h** -- *Foundation About CAN*
- [x] **canaddr.h** -- *Addresses of Nodes on CAN Buses*
- [x] **type.h** -- *Chuangix's Specific Types*
