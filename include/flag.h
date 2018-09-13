#ifndef FLAG_H_
#define FLAG_H_

#include "type.h"

typedef struct sys_flag {
        u32 vsl[2];
        u32 tls[2];
        u32 z[2];
        u32 y[4];
        u32 x[2];
        u32 prp[4];
        u32 rse[4];
        u32 swv[4];
        u32 swh[4];
        u32 gen[2];
        u32 psu[2];
        u32 mom[2];
        u32 sdt;
        u32 sds[4];
        u32 sdf[4];
        u32 sdb[4];
} SYS_FLAG;

#define UMASK_FLAG_IO 0x000000FF

#define UMASK_FLAG_FAULT   0x0000FF00
#define FLAG_FAULT_GENERAL 0x00000100
#define FLAG_FAULT_SERIOUS 0x00000200
#define FLAG_FAULT_IO      0x00000400
#define FLAG_FAULT_POS     0x00000800
#define FLAG_FAULT_VEL     0x00001000
#define FLAG_FAULT_AMPR    0x00002000
#define FLAG_FAULT_SYNC    0x00004000
#define FLAG_FAULT_COMM    0x00008000

#endif /* FLAG_H_ */
