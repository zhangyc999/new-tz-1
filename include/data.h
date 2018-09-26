#ifndef DATA_H_
#define DATA_H_

#include "type.h"

typedef struct {
        u16 head;
        u16 len;
        u32 ts;
        union {
                str data;
                struct {
                        u32 ts;
                        struct {
                                u8 code: 5;
                                u8 level: 3;
                        } fault;
                        u8 res[7];
                        s32 x;
                        s32 y;
                } tls;
                struct {
                        u32 ts;
                        struct {
                                u8 code: 5;
                                u8 level: 3;
                        } fault;
                        u8 proc;
                        u8 res[2];
                        s32 x;
                        s32 y;
                        s32 z;
                } vsl;
                struct {
                        u32 ts;
                        struct {
                                u8 code: 5;
                                u8 level: 3;
                        } fault;
                        u8 res[7];
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
                } psu;
                struct {
                        u32 ts;
                        struct {
                                u8 code: 5;
                                u8 level: 3;
                        } fault;
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
                        u8 res;
                        s32 pos;
                        s32 vel;
                        s32 ampr;
                } srv;
        } dev[48];
} DATA;

#endif /* DATA_H_ */
