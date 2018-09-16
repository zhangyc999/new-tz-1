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

#define FLAG_IO       0x000000FF
#define FLAG_IO_BRAKE 0x00000010

#define FLAG_FAULT         0x0000FF00
#define FLAG_FAULT_GENERAL 0x00000100
#define FLAG_FAULT_SERIOUS 0x00000200
#define FLAG_FAULT_IO      0x00000400
#define FLAG_FAULT_POS     0x00000800
#define FLAG_FAULT_VEL     0x00001000
#define FLAG_FAULT_AMPR    0x00002000
#define FLAG_FAULT_SYNC    0x00004000
#define FLAG_FAULT_COMM    0x00008000

#define FLAG_COMMON 0x00FF0000
#define FLAG_STOP   0x00010000
#define FLAG_START  0x00020000
#define FLAG_FINAL  0x00040000

#define FLAG_SPECIFIC  0xFF000000
#define FLAG_RSE_LOCK  0x01000000
#define FLAG_RSE_LEAVE 0x02000000
#define FLAG_RSE_JOINT 0x04000000

#endif /* FLAG_H_ */
