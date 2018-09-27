#ifndef CAN_H_
#define CAN_H_

#include "type.h"
#include "vx.h"

struct ext_can {
        NODE node;
        u8 id[4];
        u8 data[8];
        u32 ts;
};

typedef struct ext_can CAN;

#define CAN_SFF 0
#define CAN_EFF 1
#define CAN_RTR 0
#define CAN_DLC 8
#define CAN_FF  (CAN_EFF << 7 | CAN_RTR << 6 | CAN_DLC)

typedef struct {
        u8 index;
        MSG_Q_ID msg;
} ECU;

#endif /* CAN_H_ */
