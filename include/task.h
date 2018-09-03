#ifndef TASK_H_
#define TASK_H_

#include "arg.h"

struct task {
        int (*update)(struct arg, struct arg);
        int delay;
        int index;
        int tid;
#if 0
        struct arg lst;
        struct arg msg;
        struct arg rng;
        struct arg semb;
        struct arg semc;
        struct arg semm;
#endif
        struct arg info;
        struct arg recv;
        struct arg send;
};

#endif /* TASK_H_ */
