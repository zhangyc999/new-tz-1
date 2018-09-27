#ifndef DATA_H_
#define DATA_H_

#include "type.h"

typedef struct {
        u16 head;
        u16 len;
        union {
                str data;
                struct {
                        struct {
                                u8 code: 6;
                                u8 level: 2;
                        } fault;
                        u8 res[7];
                        s32 x;
                        s32 y;
                        u32 ts;
                } tls;
                struct {
                        struct {
                                u8 code: 6;
                                u8 level: 2;
                        } fault;
                        u8 proc;
                        u8 res[2];
                        s32 x;
                        s32 y;
                        s32 z;
                        u32 ts;
                } vsl;
                struct {
                        struct {
                                u8 code: 6;
                                u8 level: 2;
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
                        u32 ts;
                } psu;
                struct {
                        struct {
                                u8 code: 6;
                                u8 level: 2;
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
                        u32 ts;
                } srv;
        } dev[48];
        u32 ts;
        u8 res[3];
        u8 chk;
} DATA;

#endif /* DATA_H_ */
