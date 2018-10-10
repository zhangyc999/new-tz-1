#include "can.h"
#include "canaddr.h"
#include "cmd.h"
#include "data.h"
#include "type.h"
#include "vx.h"

#define NO  0
#define YES 1

extern int tid_core;
extern int tid_can;
extern MSG_Q_ID msg_can[2][3];
extern MSG_Q_ID msg_vsl;
extern DATA sys_data;

#define DEVS 2

#define LINK   0
#define XMIN   1
#define XMAX   2
#define YMIN   3
#define YMAX   4
#define ZMIN   5
#define ZMAX   6
#define DISMIN 7
#define DISMAX 8

void t_vsl(int period, int duration)
{
        struct {
                int tid;
                CAN *p;
        } send;
        struct {
                int xofs[DEVS];
                int yofs[DEVS];
                int zofs[DEVS];
                int xmin[DEVS][3];
                int xmax[DEVS][3];
                int ymin[DEVS][3];
                int ymax[DEVS][3];
                int zmin[DEVS][3];
                int zmax[DEVS][3];
                int dismin;
                int dismax;
        } cfg;
        int delay = period;
        int stamp;
        int tmp[DEVS];
        int ctr[DEVS] = {0, 0};
        int link[DEVS][2] = {{0}, {0}};
        int xsum[DEVS] = {0};
        int ysum[DEVS] = {0};
        int zsum[DEVS] = {0};
        int xavg[DEVS] = {0};
        int yavg[DEVS] = {0};
        int zavg[DEVS] = {0};
        int xcur[DEVS] = {0};
        int ycur[DEVS] = {0};
        int zcur[DEVS] = {0};
        int err[DEVS] = {0};
        int proc[DEVS] = {0};
        int dis = 0;
        int relax[DEVS][16] = {{0}, {0}};
        int i, j;
        int fd = open("/vsl.cfg", O_RDWR, 0644);
        u8 dev;
        CMD *cmd;
        CAN can[DEVS];
        PCAN buf[DEVS][duration];
        PCAN *p[DEVS];
        PCAN *prev[DEVS];
        LIST lst[DEVS];
        memset(buf, 0, sizeof(buf));
        cfg.xofs[0] = 0;
        cfg.xofs[1] = 0;
        cfg.yofs[0] = 0;
        cfg.yofs[1] = 0;
        cfg.zofs[0] = 0;
        cfg.zofs[1] = 0;
        cfg.xmin[0][0] = 500;
        cfg.xmin[0][1] = 1000;
        cfg.xmin[0][2] = 2000;
        cfg.xmin[1][0] = 500;
        cfg.xmin[1][1] = 1000;
        cfg.xmin[1][2] = 2000;
        cfg.xmax[0][0] = 500;
        cfg.xmax[0][1] = 1000;
        cfg.xmax[0][2] = 2000;
        cfg.xmax[1][0] = 500;
        cfg.xmax[1][1] = 1000;
        cfg.xmax[1][2] = 2000;
        cfg.ymin[0][0] = 500;
        cfg.ymin[0][1] = 1000;
        cfg.ymin[0][2] = 2000;
        cfg.ymin[1][0] = 500;
        cfg.ymin[1][1] = 1000;
        cfg.ymin[1][2] = 2000;
        cfg.ymax[0][0] = 500;
        cfg.ymax[0][1] = 1000;
        cfg.ymax[0][2] = 2000;
        cfg.ymax[1][0] = 500;
        cfg.ymax[1][1] = 1000;
        cfg.ymax[1][2] = 2000;
        cfg.dismin = 1000;
        cfg.dismax = 1000;
        if (fd)
                read(fd, (str)&cfg, sizeof(cfg));
        for (i = 0; i < DEVS; i++) {
                lstInit(&lst[i]);
                for (j = 0; j < duration; j++)
                        lstAdd(&lst[i], (NODE *)&buf[i][j]);
                lstFirst(&lst[i])->previous = lstLast(&lst[i]);
                lstLast(&lst[i])->next = lstFirst(&lst[i]);
                p[i] = (PCAN *)lstFirst(&lst[i]);
        }
        send.tid = taskIdSelf();
        for (;;) {
                stamp = tickGet();
                if (delay < 0 || delay > period)
                        delay = 0;
                if (8 == msgQReceive(msg_vsl, (str)tmp, 8, delay)) {
                        if (tmp[0] == tid_core) {
                                if (cmd->src == ((CMD *)tmp[1])->src &&
                                    cmd->dev == ((CMD *)tmp[1])->dev &&
                                    cmd->mode == ((CMD *)tmp[1])->mode &&
                                    cmd->act == ((CMD *)tmp[1])->act) {
                                        for (i = 0; i < DEVS; i++) {
                                                if (cmd->data.vsl.relax.pick & 1 << i) {
                                                        switch (cmd->data.vsl.relax.link) {
                                                        case 0:
                                                                relax[i][LINK] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][LINK] = 1;
                                                                break;
                                                        case 2:
                                                                relax[i][LINK] = 2;
                                                                break;
                                                        case 3:
                                                                relax[i][LINK] = 3;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.vsl.relax.xmin) {
                                                        case 0:
                                                                relax[i][XMIN] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][XMIN] = 100;
                                                                break;
                                                        case 2:
                                                                relax[i][XMIN] = 300;
                                                                break;
                                                        case 3:
                                                                relax[i][XMIN] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.vsl.relax.xmax) {
                                                        case 0:
                                                                relax[i][XMAX] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][XMAX] = 100;
                                                                break;
                                                        case 2:
                                                                relax[i][XMAX] = 300;
                                                                break;
                                                        case 3:
                                                                relax[i][XMAX] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.vsl.relax.ymin) {
                                                        case 0:
                                                                relax[i][YMIN] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][YMIN] = 100;
                                                                break;
                                                        case 2:
                                                                relax[i][YMIN] = 300;
                                                                break;
                                                        case 3:
                                                                relax[i][YMIN] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.vsl.relax.ymax) {
                                                        case 0:
                                                                relax[i][YMAX] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][YMAX] = 100;
                                                                break;
                                                        case 2:
                                                                relax[i][YMAX] = 300;
                                                                break;
                                                        case 3:
                                                                relax[i][YMAX] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.vsl.relax.dismin) {
                                                        case 0:
                                                                relax[i][DISMIN] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][DISMIN] = 100;
                                                                break;
                                                        case 2:
                                                                relax[i][DISMIN] = 300;
                                                                break;
                                                        case 3:
                                                                relax[i][DISMIN] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.vsl.relax.dismax) {
                                                        case 0:
                                                                relax[i][DISMAX] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][DISMAX] = 100;
                                                                break;
                                                        case 2:
                                                                relax[i][DISMAX] = 300;
                                                                break;
                                                        case 3:
                                                                relax[i][DISMAX] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                }
                                        }
                                }
                                cmd = (CMD *)tmp[1];
                                delay -= tickGet() - stamp;
                        } else if (tmp[0] == tid_can) {
                                switch (((CAN *)tmp[1])->id[0]) {
                                case CA_VSL0:
                                        dev = 0;
                                        break;
                                case CA_VSL1:
                                        dev = 1;
                                        break;
                                default:
                                        break;
                                }
                                if (p[dev]->can) {
                                        xsum[dev] -= *(s16 *)&p[dev]->can->data[0];
                                        ysum[dev] -= *(s16 *)&p[dev]->can->data[2];
                                        ctr[dev]--;
                                }
                                p[dev]->can = (CAN *)tmp[1];
                                xcur[dev] = *(s16 *)&p[dev]->can->data[0] + cfg.xofs[dev];
                                ycur[dev] = *(s16 *)&p[dev]->can->data[2] + cfg.yofs[dev];
                                zcur[dev] = *(s16 *)&p[dev]->can->data[4] + cfg.zofs[dev];
                                err[dev] = *(u8 *)&p[dev]->can->data[6];
                                proc[dev] = *(u8 *)&p[dev]->can->data[7];
                                xsum[dev] += *(s16 *)&p[dev]->can->data[0];
                                ysum[dev] += *(s16 *)&p[dev]->can->data[2];
                                zsum[dev] += *(s16 *)&p[dev]->can->data[4];
                                ctr[dev]++;
                                xavg[dev] = xsum[dev] / ctr[dev] + cfg.xofs[dev];
                                yavg[dev] = ysum[dev] / ctr[dev] + cfg.yofs[dev];
                                zavg[dev] = zsum[dev] / ctr[dev] + cfg.zofs[dev];
                                prev[dev] = (PCAN *)lstPrevious((NODE *)p[dev]);
                                switch (sys_data.vsl[dev].fault.xmin) {
                                case NORMAL:
                                        if (xcur[dev] < -10 + cfg.xmin[dev][2] - relax[dev][XMIN])
                                                sys_data.vsl[dev].fault.xmin = SERIOUS;
                                        else if (xcur[dev] < -10 + cfg.xmin[dev][1] - relax[dev][XMIN])
                                                sys_data.vsl[dev].fault.xmin = GENERAL;
                                        else if (xcur[dev] < -10 + cfg.xmin[dev][0] - relax[dev][XMIN])
                                                sys_data.vsl[dev].fault.xmin = WARNING;
                                        break;
                                case WARNING:
                                        if (xcur[dev] < -10 + cfg.xmin[dev][2] - relax[dev][XMIN])
                                                sys_data.vsl[dev].fault.xmin = SERIOUS;
                                        else if (xcur[dev] < -10 + cfg.xmin[dev][1] - relax[dev][XMIN])
                                                sys_data.vsl[dev].fault.xmin = GENERAL;
                                        else if (xcur[dev] > +10 + cfg.xmin[dev][0] - relax[dev][XMIN])
                                                sys_data.vsl[dev].fault.xmin = NORMAL;
                                        break;
                                case GENERAL:
                                        if (xcur[dev] > -10 + cfg.xmin[dev][2] - relax[dev][XMIN])
                                                sys_data.vsl[dev].fault.xmin = SERIOUS;
                                        else if (xcur[dev] > +10 + cfg.xmin[dev][0] - relax[dev][XMIN])
                                                sys_data.vsl[dev].fault.xmin = NORMAL;
                                        else if (xcur[dev] > +10 + cfg.xmin[dev][1] - relax[dev][XMIN])
                                                sys_data.vsl[dev].fault.xmin = WARNING;
                                        break;
                                case SERIOUS:
                                        if (xcur[dev] < -10 + cfg.xmin[dev][0] - relax[dev][XMIN])
                                                sys_data.vsl[dev].fault.xmin = NORMAL;
                                        else if (xcur[dev] < -10 + cfg.xmin[dev][1] - relax[dev][XMIN])
                                                sys_data.vsl[dev].fault.xmin = WARNING;
                                        else if (xcur[dev] < -10 + cfg.xmin[dev][2] - relax[dev][XMIN])
                                                sys_data.vsl[dev].fault.xmin = GENERAL;
                                        break;
                                default:
                                        break;
                                }
                                switch (sys_data.vsl[dev].fault.xmax) {
                                case NORMAL:
                                        if (xcur[dev] > cfg.xmax[dev][2] + relax[dev][XMAX])
                                                sys_data.vsl[dev].fault.xmax = SERIOUS;
                                        else if (xcur[dev] > cfg.xmax[dev][1] + relax[dev][XMAX])
                                                sys_data.vsl[dev].fault.xmax = GENERAL;
                                        else if (xcur[dev] > cfg.xmax[dev][0] + relax[dev][XMAX])
                                                sys_data.vsl[dev].fault.xmax = WARNING;
                                        break;
                                case WARNING:
                                        if (xcur[dev] > cfg.xmax[dev][2] + relax[dev][XMAX])
                                                sys_data.vsl[dev].fault.xmax = SERIOUS;
                                        else if (xcur[dev] > cfg.xmax[dev][1] + relax[dev][XMAX])
                                                sys_data.vsl[dev].fault.xmax = GENERAL;
                                        else if (xcur[dev] < -10 + cfg.xmax[dev][0] + relax[dev][XMAX])
                                                sys_data.vsl[dev].fault.xmax = NORMAL;
                                        break;
                                case GENERAL:
                                        if (xcur[dev] > cfg.xmax[dev][2] + relax[dev][XMAX])
                                                sys_data.vsl[dev].fault.xmax = SERIOUS;
                                        else if (xcur[dev] < -10 + cfg.xmax[dev][0] + relax[dev][XMAX])
                                                sys_data.vsl[dev].fault.xmax = NORMAL;
                                        else if (xcur[dev] < -10 + cfg.xmax[dev][1] + relax[dev][XMAX])
                                                sys_data.vsl[dev].fault.xmax = WARNING;
                                        break;
                                case SERIOUS:
                                        if (xcur[dev] < -10 + cfg.xmax[dev][0] + relax[dev][XMAX])
                                                sys_data.vsl[dev].fault.xmax = NORMAL;
                                        else if (xcur[dev] < -10 + cfg.xmax[dev][1] + relax[dev][XMAX])
                                                sys_data.vsl[dev].fault.xmax = WARNING;
                                        else if (xcur[dev] < -10 + cfg.xmax[dev][2] + relax[dev][XMAX])
                                                sys_data.vsl[dev].fault.xmax = GENERAL;
                                        break;
                                default:
                                        break;
                                }
                                switch (sys_data.vsl[dev].fault.y) {
                                case NORMAL:
                                        if (ycur[dev] > cfg.y[dev][2] + relax[dev][Y])
                                                sys_data.vsl[dev].fault.y = SERIOUS;
                                        else if (ycur[dev] > cfg.y[dev][1] + relax[dev][Y])
                                                sys_data.vsl[dev].fault.y = GENERAL;
                                        else if (ycur[dev] > cfg.y[dev][0] + relax[dev][Y])
                                                sys_data.vsl[dev].fault.y = WARNING;
                                        break;
                                case WARNING:
                                        if (ycur[dev] > cfg.y[dev][2] + relax[dev][Y])
                                                sys_data.vsl[dev].fault.y = SERIOUS;
                                        else if (ycur[dev] > cfg.y[dev][1] + relax[dev][Y])
                                                sys_data.vsl[dev].fault.y = GENERAL;
                                        else if (ycur[dev] < -10 + cfg.y[dev][0] + relax[dev][Y])
                                                sys_data.vsl[dev].fault.y = NORMAL;
                                        break;
                                case GENERAL:
                                        if (ycur[dev] > cfg.y[dev][2] + relax[dev][Y])
                                                sys_data.vsl[dev].fault.y = SERIOUS;
                                        else if (ycur[dev] < -10 + cfg.y[dev][0] + relax[dev][Y])
                                                sys_data.vsl[dev].fault.y = NORMAL;
                                        else if (ycur[dev] < -10 + cfg.y[dev][1] + relax[dev][Y])
                                                sys_data.vsl[dev].fault.y = WARNING;
                                        break;
                                case SERIOUS:
                                        if (ycur[dev] < -10 + cfg.y[dev][0] + relax[dev][Y])
                                                sys_data.vsl[dev].fault.y = NORMAL;
                                        else if (ycur[dev] < -10 + cfg.y[dev][1] + relax[dev][Y])
                                                sys_data.vsl[dev].fault.y = WARNING;
                                        else if (ycur[dev] < -10 + cfg.y[dev][2] + relax[dev][Y])
                                                sys_data.vsl[dev].fault.y = GENERAL;
                                        break;
                                default:
                                        break;
                                }
                                switch (sys_data.vsl[dev].fault.z) {
                                case NORMAL:
                                        if (zcur[dev] > cfg.z[dev][2] + relax[dev][Z])
                                                sys_data.vsl[dev].fault.z = SERIOUS;
                                        else if (zcur[dev] > cfg.z[dev][1] + relax[dev][Z])
                                                sys_data.vsl[dev].fault.z = GENERAL;
                                        else if (zcur[dev] > cfg.z[dev][0] + relax[dev][Z])
                                                sys_data.vsl[dev].fault.z = WARNING;
                                        break;
                                case WARNING:
                                        if (zcur[dev] > cfg.z[dev][2] + relax[dev][Z])
                                                sys_data.vsl[dev].fault.z = SERIOUS;
                                        else if (zcur[dev] > cfg.z[dev][1] + relax[dev][Z])
                                                sys_data.vsl[dev].fault.z = GENERAL;
                                        else if (zcur[dev] < -10 + cfg.z[dev][0] + relax[dev][Z])
                                                sys_data.vsl[dev].fault.z = NORMAL;
                                        break;
                                case GENERAL:
                                        if (zcur[dev] > cfg.z[dev][2] + relax[dev][Z])
                                                sys_data.vsl[dev].fault.z = SERIOUS;
                                        else if (zcur[dev] < -10 + cfg.z[dev][0] + relax[dev][Z])
                                                sys_data.vsl[dev].fault.z = NORMAL;
                                        else if (zcur[dev] < -10 + cfg.z[dev][1] + relax[dev][Z])
                                                sys_data.vsl[dev].fault.z = WARNING;
                                        break;
                                case SERIOUS:
                                        if (zcur[dev] < -10 + cfg.z[dev][0] + relax[dev][Z])
                                                sys_data.vsl[dev].fault.z = NORMAL;
                                        else if (zcur[dev] < -10 + cfg.z[dev][1] + relax[dev][Z])
                                                sys_data.vsl[dev].fault.z = WARNING;
                                        else if (zcur[dev] < -10 + cfg.z[dev][2] + relax[dev][Z])
                                                sys_data.vsl[dev].fault.z = GENERAL;
                                        break;
                                default:
                                        break;
                                }
                                switch (err[dev]) {
                                case 0x00:
                                        sys_data.vsl[dev].fault.dev = NORMAL;
                                        break;
                                case 0x03:
                                        sys_data.vsl[dev].fault.dev = 1;
                                        break;
                                case 0x0c:
                                        sys_data.vsl[dev].fault.dev = 2;
                                        break;
                                case 0xf0:
                                        sys_data.vsl[dev].fault.dev = 3;
                                        break;
                                default:
                                        break;
                                }
                                if (sys_data.vsl[0].fault.link < GENERAL && sys_data.vsl[1].fault.link < GENERAL &&
                                    sys_data.vsl[0].fault.x < GENERAL && sys_data.vsl[1].fault.x < GENERAL &&
                                    sys_data.vsl[0].fault.y < GENERAL && sys_data.vsl[1].fault.y < GENERAL &&
                                    sys_data.vsl[0].fault.z < GENERAL && sys_data.vsl[1].fault.z < GENERAL &&
                                    sys_data.vsl[0].fault.dev == NORMAL && sys_data.vsl[1].fault.dev == NORMAL) {
                                        if (p[0]->can && p[1]->can)
                                                dis = sqrt(pow(xcur[0], 2) + pow(ycur[0], 2) + pow(zcur[0], 2) +
                                                           pow(xcur[1], 2) + pow(ycur[1], 2) + pow(zcur[1], 2));
                                        else
                                                dis = 63900;
                                        for (i = 0; i < DEVS; i++) {
                                                switch (sys_data.vsl[i].fault.xd) {
                                                case NORMAL:
                                                        if (xd > cfg.xd[2] + relax[dev][XDIFF])
                                                                sys_data.vsl[i].fault.xd = SERIOUS;
                                                        else if (xd > cfg.xd[1] + relax[dev][XDIFF])
                                                                sys_data.vsl[i].fault.xd = GENERAL;
                                                        else if (xd > cfg.xd[0] + relax[dev][XDIFF])
                                                                sys_data.vsl[i].fault.xd = WARNING;
                                                        break;
                                                case WARNING:
                                                        if (xd > cfg.xd[2] + relax[dev][XDIFF])
                                                                sys_data.vsl[i].fault.xd = SERIOUS;
                                                        else if (xd > cfg.xd[1] + relax[dev][XDIFF])
                                                                sys_data.vsl[i].fault.xd = GENERAL;
                                                        else if (xd < -100 + cfg.xd[0] + relax[dev][XDIFF])
                                                                sys_data.vsl[i].fault.xd = NORMAL;
                                                        break;
                                                case GENERAL:
                                                        if (xd > cfg.xd[2] + relax[dev][XDIFF])
                                                                sys_data.vsl[i].fault.xd = SERIOUS;
                                                        else if (xd < -100 + cfg.xd[0] + relax[dev][XDIFF])
                                                                sys_data.vsl[i].fault.xd = NORMAL;
                                                        else if (xd < -100 + cfg.xd[1] + relax[dev][XDIFF])
                                                                sys_data.vsl[i].fault.xd = WARNING;
                                                        break;
                                                case SERIOUS:
                                                        if (xd < -100 + cfg.xd[0] + relax[dev][XDIFF])
                                                                sys_data.vsl[i].fault.xd = NORMAL;
                                                        else if (xd < -100 + cfg.xd[1] + relax[dev][XDIFF])
                                                                sys_data.vsl[i].fault.xd = WARNING;
                                                        else if (xd < -100 + cfg.xd[2] + relax[dev][XDIFF])
                                                                sys_data.vsl[i].fault.xd = GENERAL;
                                                        break;
                                                default:
                                                        break;
                                                }
                                                switch (sys_data.vsl[i].fault.yd) {
                                                case NORMAL:
                                                        if (yd > cfg.yd[2] + relax[dev][YDIFF])
                                                                sys_data.vsl[i].fault.yd = SERIOUS;
                                                        else if (yd > cfg.yd[1] + relax[dev][YDIFF])
                                                                sys_data.vsl[i].fault.yd = GENERAL;
                                                        else if (yd > cfg.yd[0] + relax[dev][YDIFF])
                                                                sys_data.vsl[i].fault.yd = WARNING;
                                                        break;
                                                case WARNING:
                                                        if (yd > cfg.yd[2] + relax[dev][YDIFF])
                                                                sys_data.vsl[i].fault.yd = SERIOUS;
                                                        else if (yd > cfg.yd[1] + relax[dev][YDIFF])
                                                                sys_data.vsl[i].fault.yd = GENERAL;
                                                        else if (yd < -100 + cfg.yd[0] + relax[dev][YDIFF])
                                                                sys_data.vsl[i].fault.yd = NORMAL;
                                                        break;
                                                case GENERAL:
                                                        if (yd > cfg.yd[2] + relax[dev][YDIFF])
                                                                sys_data.vsl[i].fault.yd = SERIOUS;
                                                        else if (yd < -100 + cfg.yd[0] + relax[dev][YDIFF])
                                                                sys_data.vsl[i].fault.yd = NORMAL;
                                                        else if (yd < -100 + cfg.yd[1] + relax[dev][YDIFF])
                                                                sys_data.vsl[i].fault.yd = WARNING;
                                                        break;
                                                case SERIOUS:
                                                        if (yd < -100 + cfg.yd[0] + relax[dev][YDIFF])
                                                                sys_data.vsl[i].fault.yd = NORMAL;
                                                        else if (yd < -100 + cfg.yd[1] + relax[dev][YDIFF])
                                                                sys_data.vsl[i].fault.yd = WARNING;
                                                        else if (yd < -100 + cfg.yd[2] + relax[dev][YDIFF])
                                                                sys_data.vsl[i].fault.yd = GENERAL;
                                                        break;
                                                default:
                                                        break;
                                                }
                                        }
                                } else {
                                        sys_data.vsl[0].fault.xd = NORMAL;
                                        sys_data.vsl[0].fault.yd = NORMAL;
                                        sys_data.vsl[1].fault.xd = NORMAL;
                                        sys_data.vsl[1].fault.yd = NORMAL;
                                }
                                sys_data.vsl[dev].x = xcur[dev];
                                sys_data.vsl[dev].y = ycur[dev];
                                p[dev] = (PCAN *)lstNext((NODE *)p[dev]);
                                delay -= tickGet() - stamp;
                        } else {
                                for (i = 0; i < 2; i++) {
                                        prev[i] = (PCAN *)lstPrevious((NODE *)p[i]);
                                        if (prev[i]->can) {
                                                if (tickGet() - prev[i]->can->ts < period) {
                                                        if (link[i][YES] < 10)
                                                                link[i][YES]++;
                                                        link[i][NO] = 0;
                                                } else {
                                                        if (link[i][NO] < 10)
                                                                link[i][NO]++;
                                                        link[i][YES] = 0;
                                                }
                                        } else {
                                                link[i][YES] = 10;
                                                link[i][NO] = 0;
                                        }
                                        switch (sys_data.vsl[i].fault.link) {
                                        case NORMAL:
                                                if (link[i][NO] > 2 + relax[i][LINK])
                                                        sys_data.vsl[i].fault.link = SERIOUS;
                                                else if (link[i][NO] > 1 + relax[i][LINK])
                                                        sys_data.vsl[i].fault.link = GENERAL;
                                                else if (link[i][NO] > 0 + relax[i][LINK])
                                                        sys_data.vsl[i].fault.link = WARNING;
                                                break;
                                        case WARNING:
                                                if (link[i][NO] > 2 + relax[i][LINK])
                                                        sys_data.vsl[i].fault.link = SERIOUS;
                                                else if (link[i][NO] > 1 + relax[i][LINK])
                                                        sys_data.vsl[i].fault.link = GENERAL;
                                                else if (link[i][YES] > 9 - relax[i][LINK])
                                                        sys_data.vsl[i].fault.link = NORMAL;
                                                break;
                                        case GENERAL:
                                                if (link[i][NO] > 2 + relax[i][LINK])
                                                        sys_data.vsl[i].fault.link = SERIOUS;
                                                else if (link[i][YES] > 9 - relax[i][LINK])
                                                        sys_data.vsl[i].fault.link = NORMAL;
                                                else if (link[i][YES] > 8 - relax[i][LINK])
                                                        sys_data.vsl[i].fault.link = WARNING;
                                                break;
                                        case SERIOUS:
                                                if (link[i][YES] > 9 - relax[i][LINK])
                                                        sys_data.vsl[i].fault.link = NORMAL;
                                                else if (link[i][YES] > 8 - relax[i][LINK])
                                                        sys_data.vsl[i].fault.link = WARNING;
                                                else if (link[i][YES] > 7 - relax[i][LINK])
                                                        sys_data.vsl[i].fault.link = GENERAL;
                                                break;
                                        default:
                                                break;
                                        }
                                }
                                can[0].id[0] = CA_MAIN;
                                can[0].id[1] = CA_TLS0;
                                can[0].id[2] = 0x5c;
                                can[0].id[3] = 0x0c;
                                can[0].data[0] = 0x00;
                                can[0].data[1] = 0x11;
                                can[0].data[2] = 0x22;
                                can[0].data[3] = 0x33;
                                can[0].data[4] = 0x44;
                                can[0].data[5] = 0x55;
                                can[0].data[6] = 0x66;
                                can[0].data[7] = 0x77;
                                send.p = &can[0];
                                msgQSend(msg_can[0][0], (str)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                can[1].id[0] = CA_MAIN;
                                can[1].id[1] = CA_TLS1;
                                can[1].id[2] = 0x5c;
                                can[1].id[3] = 0x0c;
                                can[1].data[0] = 0x00;
                                can[1].data[1] = 0x11;
                                can[1].data[2] = 0x22;
                                can[1].data[3] = 0x33;
                                can[1].data[4] = 0x44;
                                can[1].data[5] = 0x55;
                                can[1].data[6] = 0x66;
                                can[1].data[7] = 0x77;
                                send.p = &can[1];
                                msgQSend(msg_can[1][0], (str)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                delay = period;
                        }
                }
        }
}
