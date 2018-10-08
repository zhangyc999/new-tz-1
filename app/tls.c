#include "can.h"
#include "canaddr.h"
#include "cmd.h"
#include "data.h"
#include "vx.h"

#define NO  0
#define YES 1

extern int tid_core;
extern int tid_can;
extern MSG_Q_ID msg_can[2][3];
extern MSG_Q_ID msg_tls;
extern DATA sys_data;

void t_tls(int period, int duration)
{
        struct {
                int tid;
                CAN *p;
        } send;
        int delay = period;
        int stamp;
        int tmp[2];
        int ctr[2] = {0, 0};
        int link[2][2] = {{0}, {0}};
        int xsum[2] = {0};
        int ysum[2] = {0};
        int xavg[2] = {0};
        int yavg[2] = {0};
        int xcur[2] = {0};
        int ycur[2] = {0};
        int err[2] = {0};
        int dx[2] = {0};
        int dy[2] = {0};
        int xdiff = 0;
        int ydiff = 0;
        int rlxlink[2] = {0};
        int rlxx[2] = {0};
        int rlxy[2] = {0};
        int rlxdx[2] = {0};
        int rlxdy[2] = {0};
        int rlxxd[2] = {0};
        int rlxyd[2] = {0};
        int i;
        u8 tls;
        CMD *cmd;
        CAN can[2];
        PCAN buf[2][duration];
        PCAN *p[2];
        PCAN *prev[2];
        LIST lst[2];
        memset(buf, 0, sizeof(buf));
        lstInit(&lst[0]);
        lstInit(&lst[1]);
        for (i = 0; i < duration; i++) {
                lstAdd(&lst[0], (NODE *)&buf[0][i]);
                lstAdd(&lst[1], (NODE *)&buf[1][i]);
        }
        lstFirst(&lst[0])->previous = lstLast(&lst[0]);
        lstFirst(&lst[1])->previous = lstLast(&lst[1]);
        lstLast(&lst[0])->next = lstFirst(&lst[0]);
        lstLast(&lst[1])->next = lstFirst(&lst[1]);
        p[0] = (PCAN *)lstFirst(&lst[0]);
        p[1] = (PCAN *)lstFirst(&lst[1]);
        send.tid = taskIdSelf();
        for (;;) {
                stamp = tickGet();
                if (delay < 0 || delay > period)
                        delay = 0;
                if (8 == msgQReceive(msg_tls, (str)tmp, 8, delay)) {
                        if (tmp[0] == tid_can) {
                                switch (((CAN *)tmp[1])->id[0]) {
                                case CA_TLS0:
                                        tls = 0;
                                        break;
                                case CA_TLS1:
                                        tls = 1;
                                        break;
                                default:
                                        break;
                                }
                                if (p[tls]->can) {
                                        xsum[tls] -= *(s16 *)&p[tls]->can->data[0];
                                        ysum[tls] -= *(s16 *)&p[tls]->can->data[2];
                                        ctr[tls]--;
                                }
                                p[tls]->can = (CAN *)tmp[1];
                                prev[tls] = (PCAN *)lstPrevious((NODE *)p[tls]);
                                xcur[tls] = *(s16 *)&p[tls]->can->data[0];
                                ycur[tls] = *(s16 *)&p[tls]->can->data[2];
                                err[tls] = *(u8 *)&p[tls]->can->data[6];
                                xsum[tls] += xcur[tls];
                                ysum[tls] += ycur[tls];
                                ctr[tls]++;
                                xavg[tls] = xsum[tls] / ctr[tls];
                                yavg[tls] = ysum[tls] / ctr[tls];
                                if (prev[tls]->can) {
                                        dx[tls] = abs(xcur[tls] - * (s16 *)&prev[tls]->can->data[0]);
                                        dy[tls] = abs(ycur[tls] - * (s16 *)&prev[tls]->can->data[2]);
                                } else {
                                        dx[tls] = 0;
                                        dy[tls] = 0;
                                }
                                if (p[0]->can && p[1]->can) {
                                        xdiff = abs(xcur[0] - xcur[1]);
                                        ydiff = abs(ycur[0] - ycur[1]);
                                } else {
                                        xdiff = 0;
                                        ydiff = 0;
                                }
                                switch (sys_data.tls[tls].fault.x) {
                                case NORMAL:
                                        if (xcur[tls] > 2000 + rlxx[tls])
                                                sys_data.tls[tls].fault.x = SERIOUS;
                                        else if (xcur[tls] > 1000 + rlxx[tls])
                                                sys_data.tls[tls].fault.x = GENERAL;
                                        else if (xcur[tls] > 500 + rlxx[tls])
                                                sys_data.tls[tls].fault.x = WARNING;
                                        break;
                                case WARNING:
                                        if (xcur[tls] > 2000 + rlxx[tls])
                                                sys_data.tls[tls].fault.x = SERIOUS;
                                        else if (xcur[tls] > 1000 + rlxx[tls])
                                                sys_data.tls[tls].fault.x = GENERAL;
                                        else if (xcur[tls] < 400 + rlxx[tls])
                                                sys_data.tls[tls].fault.x = NORMAL;
                                        break;
                                case GENERAL:
                                        if (xcur[tls] > 2000 + rlxx[tls])
                                                sys_data.tls[tls].fault.x = SERIOUS;
                                        else if (xcur[tls] < 400 + rlxx[tls])
                                                sys_data.tls[tls].fault.x = NORMAL;
                                        else if (xcur[tls] < 900 + rlxx[tls])
                                                sys_data.tls[tls].fault.x = WARNING;
                                        break;
                                case SERIOUS:
                                        if (xcur[tls] < 400 + rlxx[tls])
                                                sys_data.tls[tls].fault.x = NORMAL;
                                        else if (xcur[tls] < 900 + rlxx[tls])
                                                sys_data.tls[tls].fault.x = WARNING;
                                        else if (xcur[tls] < 1900 + rlxx[tls])
                                                sys_data.tls[tls].fault.x = GENERAL;
                                        break;
                                default:
                                        break;
                                }
                                switch (sys_data.tls[tls].fault.y) {
                                case NORMAL:
                                        if (ycur[tls] > 2000 + rlxy[tls])
                                                sys_data.tls[tls].fault.y = SERIOUS;
                                        else if (ycur[tls] > 1000 + rlxy[tls])
                                                sys_data.tls[tls].fault.y = GENERAL;
                                        else if (ycur[tls] > 500 + rlxy[tls])
                                                sys_data.tls[tls].fault.y = WARNING;
                                        break;
                                case WARNING:
                                        if (ycur[tls] > 2000 + rlxy[tls])
                                                sys_data.tls[tls].fault.y = SERIOUS;
                                        else if (ycur[tls] > 1000 + rlxy[tls])
                                                sys_data.tls[tls].fault.y = GENERAL;
                                        else if (ycur[tls] < 400 + rlxy[tls])
                                                sys_data.tls[tls].fault.y = NORMAL;
                                        break;
                                case GENERAL:
                                        if (ycur[tls] > 2000 + rlxy[tls])
                                                sys_data.tls[tls].fault.y = SERIOUS;
                                        else if (ycur[tls] < 400 + rlxy[tls])
                                                sys_data.tls[tls].fault.y = NORMAL;
                                        else if (ycur[tls] < 900 + rlxy[tls])
                                                sys_data.tls[tls].fault.y = WARNING;
                                        break;
                                case SERIOUS:
                                        if (ycur[tls] < 400 + rlxy[tls])
                                                sys_data.tls[tls].fault.y = NORMAL;
                                        else if (ycur[tls] < 900 + rlxy[tls])
                                                sys_data.tls[tls].fault.y = WARNING;
                                        else if (ycur[tls] < 1900 + rlxy[tls])
                                                sys_data.tls[tls].fault.y = GENERAL;
                                        break;
                                default:
                                        break;
                                }
                                switch (sys_data.tls[tls].fault.dx) {
                                case NORMAL:
                                        if (dx[tls] > 600 + rlxdx[tls])
                                                sys_data.tls[tls].fault.dx = SERIOUS;
                                        else if (dx[tls] > 400 + rlxdx[tls])
                                                sys_data.tls[tls].fault.dx = GENERAL;
                                        else if (dx[tls] > 200 + rlxdx[tls])
                                                sys_data.tls[tls].fault.dx = WARNING;
                                        break;
                                case WARNING:
                                        if (dx[tls] > 600 + rlxdx[tls])
                                                sys_data.tls[tls].fault.dx = SERIOUS;
                                        else if (dx[tls] > 400 + rlxdx[tls])
                                                sys_data.tls[tls].fault.dx = GENERAL;
                                        else if (dx[tls] < 100 + rlxdx[tls])
                                                sys_data.tls[tls].fault.dx = NORMAL;
                                        break;
                                case GENERAL:
                                        if (dx[tls] > 600 + rlxdx[tls])
                                                sys_data.tls[tls].fault.dx = SERIOUS;
                                        else if (dx[tls] < 100 + rlxdx[tls])
                                                sys_data.tls[tls].fault.dx = NORMAL;
                                        else if (dx[tls] < 300 + rlxdx[tls])
                                                sys_data.tls[tls].fault.dx = WARNING;
                                        break;
                                case SERIOUS:
                                        if (dx[tls] < 100 + rlxdx[tls])
                                                sys_data.tls[tls].fault.dx = NORMAL;
                                        else if (dx[tls] < 300 + rlxdx[tls])
                                                sys_data.tls[tls].fault.dx = WARNING;
                                        else if (dx[tls] < 500 + rlxdx[tls])
                                                sys_data.tls[tls].fault.dx = GENERAL;
                                        break;
                                default:
                                        break;
                                }
                                switch (sys_data.tls[tls].fault.dy) {
                                case NORMAL:
                                        if (dy[tls] > 600 + rlxdy[tls])
                                                sys_data.tls[tls].fault.dy = SERIOUS;
                                        else if (dy[tls] > 400 + rlxdy[tls])
                                                sys_data.tls[tls].fault.dy = GENERAL;
                                        else if (dy[tls] > 200 + rlxdy[tls])
                                                sys_data.tls[tls].fault.dy = WARNING;
                                        break;
                                case WARNING:
                                        if (dy[tls] > 600 + rlxdy[tls])
                                                sys_data.tls[tls].fault.dy = SERIOUS;
                                        else if (dy[tls] > 400 + rlxdy[tls])
                                                sys_data.tls[tls].fault.dy = GENERAL;
                                        else if (dy[tls] < 100 + rlxdy[tls])
                                                sys_data.tls[tls].fault.dy = NORMAL;
                                        break;
                                case GENERAL:
                                        if (dy[tls] > 600 + rlxdy[tls])
                                                sys_data.tls[tls].fault.dy = SERIOUS;
                                        else if (dy[tls] < 100 + rlxdy[tls])
                                                sys_data.tls[tls].fault.dy = NORMAL;
                                        else if (dy[tls] < 300 + rlxdy[tls])
                                                sys_data.tls[tls].fault.dy = WARNING;
                                        break;
                                case SERIOUS:
                                        if (dy[tls] < 100 + rlxdy[tls])
                                                sys_data.tls[tls].fault.dy = NORMAL;
                                        else if (dy[tls] < 300 + rlxdy[tls])
                                                sys_data.tls[tls].fault.dy = WARNING;
                                        else if (dy[tls] < 500 + rlxdy[tls])
                                                sys_data.tls[tls].fault.dy = GENERAL;
                                        break;
                                default:
                                        break;
                                }
                                switch (err[tls]) {
                                case 0x00:
                                        sys_data.tls[tls].fault.dev = NORMAL;
                                        break;
                                case 0x0c:
                                        sys_data.tls[tls].fault.dev = 1;
                                        break;
                                case 0xf0:
                                        sys_data.tls[tls].fault.dev = 2;
                                        break;
                                case 0x03:
                                        sys_data.tls[tls].fault.dev = 3;
                                        break;
                                default:
                                        break;
                                }
                                if (sys_data.tls[0].fault.link < GENERAL && sys_data.tls[1].fault.link < GENERAL &&
                                    sys_data.tls[0].fault.x < GENERAL && sys_data.tls[1].fault.x < GENERAL &&
                                    sys_data.tls[0].fault.y < GENERAL && sys_data.tls[1].fault.y < GENERAL &&
                                    sys_data.tls[0].fault.dx < GENERAL && sys_data.tls[1].fault.dx < GENERAL &&
                                    sys_data.tls[0].fault.dy < GENERAL && sys_data.tls[1].fault.dy < GENERAL &&
                                    sys_data.tls[0].fault.dev == NORMAL && sys_data.tls[1].fault.dev == NORMAL) {
                                        for (i = 0; i < 2; i++) {
                                                switch (sys_data.tls[i].fault.xdiff) {
                                                case NORMAL:
                                                        if (xdiff > 2000 + rlxxd[tls])
                                                                sys_data.tls[i].fault.xdiff = SERIOUS;
                                                        else if (xdiff > 1000 + rlxxd[tls])
                                                                sys_data.tls[i].fault.xdiff = GENERAL;
                                                        else if (xdiff > 500 + rlxxd[tls])
                                                                sys_data.tls[i].fault.xdiff = WARNING;
                                                        break;
                                                case WARNING:
                                                        if (xdiff > 2000 + rlxxd[tls])
                                                                sys_data.tls[i].fault.xdiff = SERIOUS;
                                                        else if (xdiff > 1000 + rlxxd[tls])
                                                                sys_data.tls[i].fault.xdiff = GENERAL;
                                                        else if (xdiff < 400 + rlxxd[tls])
                                                                sys_data.tls[i].fault.xdiff = NORMAL;
                                                        break;
                                                case GENERAL:
                                                        if (xdiff > 2000 + rlxxd[tls])
                                                                sys_data.tls[i].fault.xdiff = SERIOUS;
                                                        else if (xdiff < 400 + rlxxd[tls])
                                                                sys_data.tls[i].fault.xdiff = NORMAL;
                                                        else if (xdiff < 900 + rlxxd[tls])
                                                                sys_data.tls[i].fault.xdiff = WARNING;
                                                        break;
                                                case SERIOUS:
                                                        if (xdiff < 400 + rlxxd[tls])
                                                                sys_data.tls[i].fault.xdiff = NORMAL;
                                                        else if (xdiff < 900 + rlxxd[tls])
                                                                sys_data.tls[i].fault.xdiff = WARNING;
                                                        else if (xdiff < 1900 + rlxxd[tls])
                                                                sys_data.tls[i].fault.xdiff = GENERAL;
                                                        break;
                                                default:
                                                        break;
                                                }
                                                switch (sys_data.tls[i].fault.ydiff) {
                                                case NORMAL:
                                                        if (ydiff > 2000 + rlxyd[tls])
                                                                sys_data.tls[i].fault.ydiff = SERIOUS;
                                                        else if (ydiff > 1000 + rlxyd[tls])
                                                                sys_data.tls[i].fault.ydiff = GENERAL;
                                                        else if (ydiff > 500 + rlxyd[tls])
                                                                sys_data.tls[i].fault.ydiff = WARNING;
                                                        break;
                                                case WARNING:
                                                        if (ydiff > 2000 + rlxyd[tls])
                                                                sys_data.tls[i].fault.ydiff = SERIOUS;
                                                        else if (ydiff > 1000 + rlxyd[tls])
                                                                sys_data.tls[i].fault.ydiff = GENERAL;
                                                        else if (ydiff < 400 + rlxyd[tls])
                                                                sys_data.tls[i].fault.ydiff = NORMAL;
                                                        break;
                                                case GENERAL:
                                                        if (ydiff > 2000 + rlxyd[tls])
                                                                sys_data.tls[i].fault.ydiff = SERIOUS;
                                                        else if (ydiff < 400 + rlxyd[tls])
                                                                sys_data.tls[i].fault.ydiff = NORMAL;
                                                        else if (ydiff < 900 + rlxyd[tls])
                                                                sys_data.tls[i].fault.ydiff = WARNING;
                                                        break;
                                                case SERIOUS:
                                                        if (ydiff < 400 + rlxyd[tls])
                                                                sys_data.tls[i].fault.ydiff = NORMAL;
                                                        else if (ydiff < 900 + rlxyd[tls])
                                                                sys_data.tls[i].fault.ydiff = WARNING;
                                                        else if (ydiff < 1900 + rlxyd[tls])
                                                                sys_data.tls[i].fault.ydiff = GENERAL;
                                                        break;
                                                default:
                                                        break;
                                                }
                                        }
                                } else {
                                        sys_data.tls[0].fault.xdiff = NORMAL;
                                        sys_data.tls[1].fault.xdiff = NORMAL;
                                        sys_data.tls[0].fault.ydiff = NORMAL;
                                        sys_data.tls[1].fault.ydiff = NORMAL;
                                }
                                sys_data.tls[tls].x = xcur[tls];
                                sys_data.tls[tls].y = ycur[tls];
                                p[tls] = (PCAN *)lstNext((NODE *)p[tls]);
                                delay -= tickGet() - stamp;
                        } else if (tmp[0] == tid_core) {
                                cmd = (CMD *)tmp[1];
                                if (cmd->src == ((CMD *)tmp[1])->src &&
                                    cmd->dev == ((CMD *)tmp[1])->dev &&
                                    cmd->mode == ((CMD *)tmp[1])->mode &&
                                    cmd->act == ((CMD *)tmp[1])->act) {
                                        for (i = 0; i < 2; i++) {
                                                if (cmd->data.tls.relax.pick & 1 << i) {
                                                        switch (cmd->data.tls.relax.link) {
                                                        case 0:
                                                                rlxlink[i] = 0;
                                                                break;
                                                        case 1:
                                                                rlxlink[i] = 1;
                                                                break;
                                                        case 2:
                                                                rlxlink[i] = 2;
                                                                break;
                                                        case 3:
                                                                rlxlink[i] = 3;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                }
                                                if (cmd->data.tls.relax.pick & 1 << i) {
                                                        switch (cmd->data.tls.relax.x) {
                                                        case 0:
                                                                rlxx[i] = 0;
                                                                break;
                                                        case 1:
                                                                rlxx[i] = 100;
                                                                break;
                                                        case 2:
                                                                rlxx[i] = 300;
                                                                break;
                                                        case 3:
                                                                rlxx[i] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.y) {
                                                        case 0:
                                                                rlxy[i] = 0;
                                                                break;
                                                        case 1:
                                                                rlxy[i] = 100;
                                                                break;
                                                        case 2:
                                                                rlxy[i] = 300;
                                                                break;
                                                        case 3:
                                                                rlxy[i] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.dx) {
                                                        case 0:
                                                                rlxdx[i] = 0;
                                                                break;
                                                        case 1:
                                                                rlxdx[i] = 100;
                                                                break;
                                                        case 2:
                                                                rlxdx[i] = 300;
                                                                break;
                                                        case 3:
                                                                rlxdx[i] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.dy) {
                                                        case 0:
                                                                rlxdy[i] = 0;
                                                                break;
                                                        case 1:
                                                                rlxdy[i] = 100;
                                                                break;
                                                        case 2:
                                                                rlxdy[i] = 300;
                                                                break;
                                                        case 3:
                                                                rlxdy[i] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.xdiff) {
                                                        case 0:
                                                                rlxxd[i] = 0;
                                                                break;
                                                        case 1:
                                                                rlxxd[i] = 100;
                                                                break;
                                                        case 2:
                                                                rlxxd[i] = 300;
                                                                break;
                                                        case 3:
                                                                rlxxd[i] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.ydiff) {
                                                        case 0:
                                                                rlxyd[i] = 0;
                                                                break;
                                                        case 1:
                                                                rlxyd[i] = 100;
                                                                break;
                                                        case 2:
                                                                rlxyd[i] = 300;
                                                                break;
                                                        case 3:
                                                                rlxyd[i] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                }
                                        }
                                }
                                delay -= tickGet() - stamp;
                        } else {
                                for (i = 0; i < 2; i++) {
                                        if (prev[i]->can) {
                                                if (tickGet() - prev[i]->can->ts < period * 1.1) {
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
                                        switch (sys_data.tls[i].fault.link) {
                                        case NORMAL:
                                                if (link[i][NO] > 2 + rlxlink[i])
                                                        sys_data.tls[i].fault.link = SERIOUS;
                                                else if (link[i][NO] > 1 + rlxlink[i])
                                                        sys_data.tls[i].fault.link = GENERAL;
                                                else if (link[i][NO] > 0 + rlxlink[i])
                                                        sys_data.tls[i].fault.link = WARNING;
                                                break;
                                        case WARNING:
                                                if (link[i][NO] > 2 + rlxlink[i])
                                                        sys_data.tls[i].fault.link = SERIOUS;
                                                else if (link[i][NO] > 1 + rlxlink[i])
                                                        sys_data.tls[i].fault.link = GENERAL;
                                                else if (link[i][YES] > 9 - rlxlink[i])
                                                        sys_data.tls[i].fault.link = NORMAL;
                                                break;
                                        case GENERAL:
                                                if (link[i][NO] > 2 + rlxlink[i])
                                                        sys_data.tls[i].fault.link = SERIOUS;
                                                else if (link[i][YES] > 9 - rlxlink[i])
                                                        sys_data.tls[i].fault.link = NORMAL;
                                                else if (link[i][YES] > 8 - rlxlink[i])
                                                        sys_data.tls[i].fault.link = WARNING;
                                                break;
                                        case SERIOUS:
                                                if (link[i][YES] > 9 - rlxlink[i])
                                                        sys_data.tls[i].fault.link = NORMAL;
                                                else if (link[i][YES] > 8 - rlxlink[i])
                                                        sys_data.tls[i].fault.link = WARNING;
                                                else if (link[i][YES] > 7 - rlxlink[i])
                                                        sys_data.tls[i].fault.link = GENERAL;
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
