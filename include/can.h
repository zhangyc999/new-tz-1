#ifndef CAN_H_
#define CAN_H_

#include "type.h"

#define CAN_SFF 0
#define CAN_EFF 1
#define CAN_RTR 0
#define CAN_DLC 8
#define CAN_FF  (CAN_EFF << 7 | CAN_RTR << 6 | CAN_DLC)

struct ext_can {
        u8 id[4];
        u8 data[8];
        u32 ts;
};

#endif /* CAN_H_ */
