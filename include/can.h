#ifndef CAN_H_
#define CAN_H_

#include "vx.h"

#define SFF 0
#define EFF 1
#define RTR 0
#define DLC 8

#define CAN_FF (EFF << 7 | RTR << 6 | DLC)

struct ext {
        NODE node;
        unsigned char id[4];
        unsigned char data[8];
        unsigned ts;
};

struct can {
        NODE node;
        struct ext *can;
};

struct ecu {
        unsigned char index;
        MSG_Q_ID msg;
};

#endif /* CAN_H_ */
