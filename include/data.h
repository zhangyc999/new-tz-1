#ifndef DATA_H_
#define DATA_H_

#include "type.h"

typedef struct {
        u16 head;
        u16 len;
        union {
                str data;
                struct {
                        s16 x;
                        s16 y;
                        u8 res[7];
                        u8 err;
                        u32 errno;
                        u32 ts;
                } tls;
                struct {
                        s16 x;
                        s16 y;
                        s16 z;
                        u8 proc;
                        u8 res[4];
                        u8 err;
                        u32 errno;
                        u32 ts;
                } vsl;
                struct {
                        struct {
                                u32 : 8;
                                u32 : 1;
                                u32 light: 1;
                                u32 m5: 1;
                                u32 m7: 1;
                                u32 : 4;
                                u32 mom: 1;
                                u32 shdb: 1;
                                u32 shdf: 1;
                                u32 : 5;
                                u32 shdts: 1;
                                u32 leg0: 1;
                                u32 leg3: 1;
                                u32 leg1: 1;
                                u32 leg2: 1;
                                u32 xyzb: 1;
                                u32 xyzf: 1;
                                u32 : 1;
                        } v24;
                        struct {
                                u32 : 8;
                                u32 : 8;
                                u32 mom: 1;
                                u32 shdb: 1;
                                u32 shdf: 1;
                                u32 : 5;
                                u32 shdts: 1;
                                u32 leg0: 1;
                                u32 leg3: 1;
                                u32 leg1: 1;
                                u32 leg2: 1;
                                u32 xyzb: 1;
                                u32 xyzf: 1;
                                u32 : 1;
                        } v500;
                        u8 res[3];
                        u8 err;
                        u32 errno;
                        u32 ts;
                } psu;
                struct {
                        s16 pos;
                        s16 vel;
                        s16 ampr;
                        struct {
                                u8 brake: 1;
                                u8 min: 1;
                                u8 max: 1;
                                u8 res: 5;
                        } io;
                        struct {
                                u8 min: 1;
                                u8 max: 1;
                                u8 mida: 1;
                                u8 midb: 1;
                                u8 stop: 1;
                                u8 loadp: 1;
                                u8 loadn: 1;
                                u8 lock: 1;
                        } misc;
                        u8 res[3];
                        u8 err;
                        u32 errno;
                        u32 ts;
                } srv;
        } dev[48];
        u32 ts;
        u8 res[3];
        u8 chk;
} DATA;

#endif /* DATA_H_ */
