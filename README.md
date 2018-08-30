# new-tz-1
The Main Controller Program of Large Tonnage Transshipping Vehicle

### include/
- [x] **type.h** -- *Chuangix's Specific Types*
- [x] **can.h**
- [x] **canaddr.h** -- *Addresses of Nodes on CAN Buses*
- [x] **candev.h**
### app/
- [ ] **init.c** -- *Initialization of Whole System*
- [x] **can.c** -- *Task of Communication on CAN Bus*
  - [x] **void task_can()**
  - [x] **static void init0()**
  - [x] **static void init1()**
  - [x] **static void isr_rx0()**
  - [x] **static void isr_rx1()**
- [ ] **udp.c** -- *Tasks of Communication on Ethernet With UDP Protocol*
- [ ] **core.c** -- *Task to Schedule All Other Tasks*
- [ ] **power.c** -- *Task of Power Supply*
- [ ] **tilt.c** -- *Task to Deal With Two Tilt Sensors*
- [ ] **visual.c** -- *Task to Deal With Two Visual Locators*
- [ ] **servo.c** -- *Task to Deal With a Single Servo Driver*
- [ ] **unite.c** -- *Task to Deal With Multiple Servo Drivers*
- [ ] **sync.c** -- *A Compensator to Multiple Servo Drivers Synchronization*
- [ ] **level.c** -- *A Compensator for Four-Point Support Leveling*
- [ ] **debug.c** -- *Task to Display Debug Messsages*
- [ ] **cmd.c** -- *Dummy of Manipulator*
### common/
- [ ] **chkcomm.c** -- *Check Communication*
