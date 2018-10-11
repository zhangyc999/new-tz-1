#ifndef DATA_H_
#define DATA_H_

struct data {
        unsigned short head;
        unsigned short len;
        struct {
                struct {
                        unsigned char link: 2;
                        unsigned char pmin: 2;
                        unsigned char pmax: 2;
                        unsigned char rmin: 2;
                        unsigned char rmax: 2;
                        unsigned char dp: 2;
                        unsigned char dr: 2;
                        unsigned char pdiff: 2;
                        unsigned char rdiff: 2;
                        unsigned char : 6;
                        unsigned char : 6;
                        unsigned char dev: 2;
                } fault;
                short p;
                short r;
                unsigned ts;
        } tls[2];
        struct {
                struct {
                        unsigned char link: 2;
                        unsigned char xmin: 2;
                        unsigned char xmax: 2;
                        unsigned char ymin: 2;
                        unsigned char ymax: 2;
                        unsigned char zmin: 2;
                        unsigned char zmax: 2;
                        unsigned char dmin: 2;
                        unsigned char dmax: 2;
                        unsigned char : 6;
                        unsigned char : 6;
                        unsigned char dev: 2;
                } fault;
                unsigned char res;
                unsigned char proc;
                short x;
                short y;
                short z;
                unsigned ts;
        } vsl[2];
        struct {
                struct {
                        unsigned char link: 2;
                        unsigned char v24min: 2;
                        unsigned char v24max: 2;
                        unsigned char a24min: 2;
                        unsigned char a24max: 2;
                        unsigned char v500min: 2;
                        unsigned char v500max: 2;
                        unsigned char a500min: 2;
                        unsigned char a500max: 2;
                        unsigned char dv24: 2;
                        unsigned char da24: 2;
                        unsigned char dv500: 2;
                        unsigned char da500: 2;
                        unsigned char : 4;
                        unsigned char dev: 2;
                } fault;
                struct {
                        unsigned char : 8;
                        unsigned char : 1;
                        unsigned char light: 1;
                        unsigned char m5: 1;
                        unsigned char m7: 1;
                        unsigned char : 4;
                        unsigned char mom: 1;
                        unsigned char shdb: 1;
                        unsigned char shdf: 1;
                        unsigned char : 5;
                        unsigned char shdst: 1;
                        unsigned char leg0: 1;
                        unsigned char leg3: 1;
                        unsigned char leg1: 1;
                        unsigned char leg2: 1;
                        unsigned char xyzb: 1;
                        unsigned char xyzf: 1;
                        unsigned char : 1;
                        unsigned short volt;
                        unsigned short ampr;
                } v24;
                struct {
                        unsigned char : 8;
                        unsigned char : 8;
                        unsigned char mom: 1;
                        unsigned char shdb: 1;
                        unsigned char shdf: 1;
                        unsigned char : 5;
                        unsigned char shdst: 1;
                        unsigned char leg0: 1;
                        unsigned char leg3: 1;
                        unsigned char leg1: 1;
                        unsigned char leg2: 1;
                        unsigned char xyzb: 1;
                        unsigned char xyzf: 1;
                        unsigned char : 1;
                        unsigned short volt;
                        unsigned short ampr;
                } v500;
                unsigned ts;
        } psu;
        struct {
                struct {
                        unsigned char link: 2;
                        unsigned char sync: 2;
                        unsigned char pmin: 2;
                        unsigned char pmax: 2;
                        unsigned char vmin: 2;
                        unsigned char vmax: 2;
                        unsigned char amin: 2;
                        unsigned char amax: 2;
                        unsigned char dp: 2;
                        unsigned char dv: 2;
                        unsigned char da: 2;
                        unsigned char smin: 2;
                        unsigned char smax: 2;
                        unsigned char brake: 2;
                        unsigned char limit: 2;
                        unsigned char dev: 2;
                } fault;
                struct {
                        unsigned char brake: 1;
                        unsigned char min: 1;
                        unsigned char max: 1;
                        unsigned char : 5;
                } io;
                struct {
                        unsigned char min: 1;
                        unsigned char max: 1;
                        unsigned char mida: 1;
                        unsigned char midb: 1;
                        unsigned char stop: 1;
                        unsigned char loadp: 1;
                        unsigned char loadn: 1;
                        unsigned char lock: 1;
                } misc;
                unsigned char res[3];
                unsigned char err;
                short pos;
                short vel;
                short ampr;
                unsigned ts;
        } srv[41];
        unsigned ts;
        struct {
                unsigned char bus0: 1;
                unsigned char bus1: 1;
                unsigned char : 6;
        } fault;
        unsigned char res[2];
        unsigned char chk;
};

#define NORMAL  0
#define WARNING 1
#define GENERAL 2
#define SERIOUS 3

#endif /* DATA_H_ */
