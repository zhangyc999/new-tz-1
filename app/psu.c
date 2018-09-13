#include "type.h"
#include "vx.h"

void t_psu(int period_slow, int period_fast, int prio_low, int prio_high)
{
        int tid = taskIdSelf();
        int prio = prio_low;
        int period = period_slow;
        for (;;) {
        }
}
