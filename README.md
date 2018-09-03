# new-tz-1
The Main Controller Program of Large Tonnage Transshipping Vehicle

### app/
- [x] **can.c** -- *Task of Communication on CAN Bus*
  - [x] **static void init0()**
  - [x] **static void init1()**
  - [x] **static void isr_rx0()**
  - [x] **static void isr_rx1()**
  - [x] **void task_can()**
- [ ] **cmd.c** -- *Dummy of Manipulator*
- [ ] **core.c** -- *Task to Schedule All Other Tasks*
- [ ] **debug.c** -- *Task to Display Debug Messsages*
- [ ] **init.c** -- *Initialization of Whole System*
- [ ] **level.c** -- *A Compensator for Four-Point Support Leveling*
- [ ] **power.c** -- *Task of Power Supply*
- [ ] **servo.c** -- *Task to Deal With a Single Servo Driver*
- [ ] **sync.c** -- *A Compensator to Multiple Servo Drivers Synchronization*
- [ ] **tilt.c** -- *Task to Deal With Two Tilt Sensors*
- [ ] **udp.c** -- *Tasks of Communication on Ethernet With UDP Protocol*
- [ ] **unite.c** -- *Task to Deal With Multiple Servo Drivers*
- [ ] **visual.c** -- *Task to Deal With Two Visual Locators*
### common/
- [ ] **chkcomm.c** -- *Check Communication*
### include/
- [x] **arg.h** -- *A General Common Structure of Argument*
- [x] **can.h** -- *Foundation About CAN*
- [x] **canaddr.h** -- *Addresses of Nodes on CAN Buses*
- [ ] **task.h** -- *A General Common Structure of Task*
- [x] **type.h** -- *Chuangix's Specific Types*
