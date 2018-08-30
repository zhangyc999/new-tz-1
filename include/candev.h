#ifndef CANDEV_H_
#define CANDEV_H_

#include "type.h"

struct can_dev {
        MSG_ID msg;
        int delay;
        int key;
        int prio;
        int tid;
        u8 ca;
};

#endif /* CANDEV_H_ */
