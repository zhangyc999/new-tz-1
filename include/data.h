#ifndef DATA_H_
#define DATA_H_

#include "type.h"

typedef struct {
        u16 head;
        u16 len;
        struct {
                struct {
                        u8 link: 2;
                        u8 xmin: 2;
                        u8 xmax: 2;
                        u8 ymin: 2;
                        u8 ymax: 2;
                        u8 dx: 2;
                        u8 dy: 2;
                        u8 xd: 2;
                        u8 yd: 2;
                        u8 : 6;
                        u8 : 6;
                        u8 dev: 2;
                } fault;
                s16 x;
                s16 y;
                u32 ts;
        } tls[2];
        struct {
                struct {
                        u8 link: 2;
                        u8 xmin: 2;
                        u8 xmax: 2;
                        u8 ymin: 2;
                        u8 ymax: 2;
                        u8 zmin: 2;
                        u8 zmax: 2;
                        u8 dismin: 2;
                        u8 dismax: 2;
                        u8 : 6;
                        u8 : 6;
                        u8 dev: 2;
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
                        u8 link: 2;
                        u8 v24min: 2;
                        u8 v24max: 2;
                        u8 a24min: 2;
                        u8 a24max: 2;
                        u8 v500min: 2;
                        u8 v500max: 2;
                        u8 a500min: 2;
                        u8 a500max: 2;
                        u8 dv24: 2;
                        u8 da24: 2;
                        u8 dv500: 2;
                        u8 da500: 2;
                        u8 : 4;
                        u8 dev: 2;
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
                        u8 shdst: 1;
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
                        u8 shdst: 1;
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
                        u8 link: 2;
                        u8 sync: 2;
                        u8 pmin: 2;
                        u8 pmax: 2;
                        u8 vel: 2;
                        u8 amin: 2;
                        u8 amax: 2;
                        u8 dp: 2;
                        u8 dv: 2;
                        u8 da: 2;
                        u8 smin: 2;
                        u8 smax: 2;
                        u8 brake: 2;
                        u8 limit: 2;
                        u8 : 2;
                        u8 dev: 2;
                } fault;
                struct {
                        u8 brake: 1;
                        u8 min: 1;
                        u8 max: 1;
                        u8 : 5;
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
        struct {
                u8 bus0: 1;
                u8 bus1: 1;
                u8 : 6;
        } fault;
        u8 res[2];
        u8 chk;
} DATA;

#define NORMAL  0
#define WARNING 1
#define GENERAL 2
#define SERIOUS 3

#endif /* DATA_H_ */
