#include "type.h"
#include "vx.h"

void t_psu(int prio_low, int prio_high, int period_slow, int period_fast)
{
        int tid = taskIdSelf();
        int prio = prio_low;
        int period = period_slow;
        for (;;) {
        }
}
