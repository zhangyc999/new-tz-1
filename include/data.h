#ifndef DATA_H_
#define DATA_H_

#include "type.h"

typedef struct {
        u16 head;
        u16 len;
        struct {
                struct {
                        u8 err0: 2;
                        u8 err1: 2;
                        u8 err2: 2;
                        u8 err3: 2;
                        u8 err4: 2;
                        u8 err5: 2;
                        u8 err6: 2;
                        u8 err7: 2;
                        u8 err8: 2;
                        u8 err9: 2;
                        u8 err10: 2;
                        u8 err11: 2;
                        u8 err12: 2;
                        u8 err13: 2;
                        u8 err14: 2;
                        u8 err15: 2;
                } fault;
                s16 x;
                s16 y;
                u32 ts;
        } tls[2];
        struct {
                struct {
                        u8 err0: 2;
                        u8 err1: 2;
                        u8 err2: 2;
                        u8 err3: 2;
                        u8 err4: 2;
                        u8 err5: 2;
                        u8 err6: 2;
                        u8 err7: 2;
                        u8 err8: 2;
                        u8 err9: 2;
                        u8 err10: 2;
                        u8 err11: 2;
                        u8 err12: 2;
                        u8 err13: 2;
                        u8 err14: 2;
                        u8 err15: 2;
                } fault;
                u8 res;
                u8 proc;
                s16 x;
                s16 y;
                s16 z;
                u32 ts;
        } vsl[2];
        struct {
                struct {
                        u8 err0: 2;
                        u8 err1: 2;
                        u8 err2: 2;
                        u8 err3: 2;
                        u8 err4: 2;
                        u8 err5: 2;
                        u8 err6: 2;
                        u8 err7: 2;
                        u8 err8: 2;
                        u8 err9: 2;
                        u8 err10: 2;
                        u8 err11: 2;
                        u8 err12: 2;
                        u8 err13: 2;
                        u8 err14: 2;
                        u8 err15: 2;
                } fault;
                struct {
                        u8 : 8;
                        u8 : 1;
                        u8 light: 1;
                        u8 m5: 1;
                        u8 m7: 1;
                        u8 : 4;
                        u8 mom: 1;
                        u8 shdb: 1;
                        u8 shdf: 1;
                        u8 : 5;
                        u8 shdts: 1;
                        u8 leg0: 1;
                        u8 leg3: 1;
                        u8 leg1: 1;
                        u8 leg2: 1;
                        u8 xyzb: 1;
                        u8 xyzf: 1;
                        u8 : 1;
                        u16 volt;
                        u16 ampr;
                } v24;
                struct {
                        u8 : 8;
                        u8 : 8;
                        u8 mom: 1;
                        u8 shdb: 1;
                        u8 shdf: 1;
                        u8 : 5;
                        u8 shdts: 1;
                        u8 leg0: 1;
                        u8 leg3: 1;
                        u8 leg1: 1;
                        u8 leg2: 1;
                        u8 xyzb: 1;
                        u8 xyzf: 1;
                        u8 : 1;
                        u16 volt;
                        u16 ampr;
                } v500;
                u32 ts;
        } psu;
        struct {
                struct {
                        u8 err0: 2;
                        u8 err1: 2;
                        u8 err2: 2;
                        u8 err3: 2;
                        u8 err4: 2;
                        u8 err5: 2;
                        u8 err6: 2;
                        u8 err7: 2;
                        u8 err8: 2;
                        u8 err9: 2;
                        u8 err10: 2;
                        u8 err11: 2;
                        u8 err12: 2;
                        u8 err13: 2;
                        u8 err14: 2;
                        u8 err15: 2;
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
                u8 res[3];
                u8 err;
                s16 pos;
                s16 vel;
                s16 ampr;
                u32 ts;
        } srv[41];
        u32 ts;
        u8 res[3];
        u8 chk;
} DATA;

#endif /* DATA_H_ */
