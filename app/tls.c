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
extern MSG_Q_ID msg_tls;
extern DATA sys_data;

#define DEVS 2

#define LINK  0
#define X     1
#define Y     2
#define DX    3
#define DY    4
#define XDIFF 5
#define YDIFF 6

void t_tls(int period, int duration)
{
        struct {
                int tid;
                CAN *p;
        } send;
        struct {
                int ofs[DEVS][2];
                int x[DEVS][3];
                int y[DEVS][3];
                int dx[DEVS][3];
                int dy[DEVS][3];
                int xd[3];
                int yd[3];
        } cfg;
        int delay = period;
        int stamp;
        int tmp[DEVS];
        int ctr[DEVS] = {0, 0};
        int link[DEVS][2] = {{0}, {0}};
        int sum[DEVS][2] = {{0}, {0}};
        int avg[DEVS][2] = {{0}, {0}};
        int cur[DEVS][2] = {{0}, {0}};
        int err[DEVS] = {0};
        int delta[DEVS][2] = {{0}, {0}};
        int xd = 0;
        int yd = 0;
        int relax[DEVS][16] = {{0}, {0}};
        int i, j;
        int fd = open("/tls.cfg", O_RDWR, 0644);
        u8 dev;
        CMD *cmd;
        CAN can[DEVS];
        PCAN buf[DEVS][duration];
        PCAN *p[DEVS];
        PCAN *prev[DEVS];
        LIST lst[DEVS];
        memset(buf, 0, sizeof(buf));
        cfg.ofs[0][0] = 0;
        cfg.ofs[0][1] = 0;
        cfg.ofs[1][0] = 0;
        cfg.ofs[1][1] = 0;
        cfg.x[0][0] = 500;
        cfg.x[0][1] = 1000;
        cfg.x[0][2] = 2000;
        cfg.x[1][0] = 500;
        cfg.x[1][1] = 1000;
        cfg.x[1][2] = 2000;
        cfg.y[0][0] = 500;
        cfg.y[0][1] = 1000;
        cfg.y[0][2] = 2000;
        cfg.y[1][0] = 500;
        cfg.y[1][1] = 1000;
        cfg.y[1][2] = 2000;
        cfg.dx[0][0] = 200;
        cfg.dx[0][1] = 500;
        cfg.dx[0][2] = 1000;
        cfg.dx[1][0] = 200;
        cfg.dx[1][1] = 500;
        cfg.dx[1][2] = 1000;
        cfg.xd[0] = 200;
        cfg.xd[1] = 500;
        cfg.xd[2] = 1000;
        cfg.yd[0] = 200;
        cfg.yd[1] = 500;
        cfg.yd[2] = 1000;
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
                if (8 == msgQReceive(msg_tls, (str)tmp, 8, delay)) {
                        if (tmp[0] == tid_core) {
                                cmd = (CMD *)tmp[1];
                                if (cmd->src == ((CMD *)tmp[1])->src &&
                                    cmd->dev == ((CMD *)tmp[1])->dev &&
                                    cmd->mode == ((CMD *)tmp[1])->mode &&
                                    cmd->act == ((CMD *)tmp[1])->act) {
                                        for (i = 0; i < DEVS; i++) {
                                                if (cmd->data.tls.relax.pick & 1 << i) {
                                                        switch (cmd->data.tls.relax.link) {
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
                                                        switch (cmd->data.tls.relax.x) {
                                                        case 0:
                                                                relax[i][X] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][X] = 100;
                                                                break;
                                                        case 2:
                                                                relax[i][X] = 300;
                                                                break;
                                                        case 3:
                                                                relax[i][X] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.y) {
                                                        case 0:
                                                                relax[i][Y] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][Y] = 100;
                                                                break;
                                                        case 2:
                                                                relax[i][Y] = 300;
                                                                break;
                                                        case 3:
                                                                relax[i][Y] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.dx) {
                                                        case 0:
                                                                relax[i][DX] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][DX] = 100;
                                                                break;
                                                        case 2:
                                                                relax[i][DX] = 300;
                                                                break;
                                                        case 3:
                                                                relax[i][DX] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.dy) {
                                                        case 0:
                                                                relax[i][DY] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][DY] = 100;
                                                                break;
                                                        case 2:
                                                                relax[i][DY] = 300;
                                                                break;
                                                        case 3:
                                                                relax[i][DY] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.xdiff) {
                                                        case 0:
                                                                relax[i][XDIFF] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][XDIFF] = 100;
                                                                break;
                                                        case 2:
                                                                relax[i][XDIFF] = 300;
                                                                break;
                                                        case 3:
                                                                relax[i][XDIFF] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.ydiff) {
                                                        case 0:
                                                                relax[i][YDIFF] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][YDIFF] = 100;
                                                                break;
                                                        case 2:
                                                                relax[i][YDIFF] = 300;
                                                                break;
                                                        case 3:
                                                                relax[i][YDIFF] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                }
                                        }
                                }
                                delay -= tickGet() - stamp;
                        } else if (tmp[0] == tid_can) {
                                switch (((CAN *)tmp[1])->id[0]) {
                                case CA_TLS0:
                                        dev = 0;
                                        break;
                                case CA_TLS1:
                                        dev = 1;
                                        break;
                                default:
                                        break;
                                }
                                if (p[dev]->can) {
                                        sum[dev][0] -= *(s16 *)&p[dev]->can->data[0];
                                        sum[dev][1] -= *(s16 *)&p[dev]->can->data[2];
                                        ctr[dev]--;
                                }
                                p[dev]->can = (CAN *)tmp[1];
                                cur[dev][0] = *(s16 *)&p[dev]->can->data[0] + cfg.ofs[dev][0];
                                cur[dev][1] = *(s16 *)&p[dev]->can->data[2] + cfg.ofs[dev][1];
                                err[dev] = *(u8 *)&p[dev]->can->data[6];
                                sum[dev][0] += *(s16 *)&p[dev]->can->data[0];
                                sum[dev][1] += *(s16 *)&p[dev]->can->data[2];
                                ctr[dev]++;
                                avg[dev][0] = sum[dev][0] / ctr[dev] + cfg.ofs[dev][0];
                                avg[dev][1] = sum[dev][1] / ctr[dev] + cfg.ofs[dev][1];
                                prev[dev] = (PCAN *)lstPrevious((NODE *)p[dev]);
                                if (prev[dev]->can) {
                                        delta[dev][0] = abs(cur[dev][0] - * (s16 *)&prev[dev]->can->data[0]);
                                        delta[dev][1] = abs(cur[dev][1] - * (s16 *)&prev[dev]->can->data[2]);
                                } else {
                                        delta[dev][0] = 0;
                                        delta[dev][1] = 0;
                                }
                                switch (sys_data.tls[dev].fault.x) {
                                case NORMAL:
                                        if (cur[dev][0] > cfg.x[dev][2] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = SERIOUS;
                                        else if (cur[dev][0] > cfg.x[dev][1] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = GENERAL;
                                        else if (cur[dev][0] > cfg.x[dev][0] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = WARNING;
                                        break;
                                case WARNING:
                                        if (cur[dev][0] > cfg.x[dev][2] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = SERIOUS;
                                        else if (cur[dev][0] > cfg.x[dev][1] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = GENERAL;
                                        else if (cur[dev][0] < -100 + cfg.x[dev][0] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = NORMAL;
                                        break;
                                case GENERAL:
                                        if (cur[dev][0] > cfg.x[dev][2] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = SERIOUS;
                                        else if (cur[dev][0] < -100 + cfg.x[dev][0] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = NORMAL;
                                        else if (cur[dev][0] < -100 + cfg.x[dev][1] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = WARNING;
                                        break;
                                case SERIOUS:
                                        if (cur[dev][0] < -100 + cfg.x[dev][0] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = NORMAL;
                                        else if (cur[dev][0] < -100 + cfg.x[dev][1] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = WARNING;
                                        else if (cur[dev][0] < -100 + cfg.x[dev][2] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = GENERAL;
                                        break;
                                default:
                                        break;
                                }
                                switch (sys_data.tls[dev].fault.y) {
                                case NORMAL:
                                        if (cur[dev][1] > cfg.y[dev][2] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = SERIOUS;
                                        else if (cur[dev][1] > cfg.y[dev][1] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = GENERAL;
                                        else if (cur[dev][1] > cfg.y[dev][0] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = WARNING;
                                        break;
                                case WARNING:
                                        if (cur[dev][1] > cfg.y[dev][2] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = SERIOUS;
                                        else if (cur[dev][1] > cfg.y[dev][1] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = GENERAL;
                                        else if (cur[dev][1] < -100 + cfg.y[dev][0] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = NORMAL;
                                        break;
                                case GENERAL:
                                        if (cur[dev][1] > cfg.y[dev][2] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = SERIOUS;
                                        else if (cur[dev][1] < -100 + cfg.y[dev][0] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = NORMAL;
                                        else if (cur[dev][1] < -100 + cfg.y[dev][1] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = WARNING;
                                        break;
                                case SERIOUS:
                                        if (cur[dev][1] < -100 + cfg.y[dev][0] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = NORMAL;
                                        else if (cur[dev][1] < -100 + cfg.y[dev][1] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = WARNING;
                                        else if (cur[dev][1] < -100 + cfg.y[dev][2] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = GENERAL;
                                        break;
                                default:
                                        break;
                                }
                                switch (sys_data.tls[dev].fault.dx) {
                                case NORMAL:
                                        if (delta[dev][0] > cfg.dx[dev][2] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dx = SERIOUS;
                                        else if (delta[dev][0] > cfg.dx[dev][1] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dx = GENERAL;
                                        else if (delta[dev][0] > cfg.dx[dev][0] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dx = WARNING;
                                        break;
                                case WARNING:
                                        if (delta[dev][0] > cfg.dx[dev][2] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dx = SERIOUS;
                                        else if (delta[dev][0] > cfg.dx[dev][1] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dx = GENERAL;
                                        else if (delta[dev][0] < -100 + cfg.dx[dev][0] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dx = NORMAL;
                                        break;
                                case GENERAL:
                                        if (delta[dev][0] > cfg.dx[dev][2] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dx = SERIOUS;
                                        else if (delta[dev][0] < -100 + cfg.dx[dev][0] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dx = NORMAL;
                                        else if (delta[dev][0] < -100 + cfg.dx[dev][1] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dx = WARNING;
                                        break;
                                case SERIOUS:
                                        if (delta[dev][0] < -100 + cfg.dx[dev][0] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dx = NORMAL;
                                        else if (delta[dev][0] < -100 + cfg.dx[dev][1] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dx = WARNING;
                                        else if (delta[dev][0] < -100 + cfg.dx[dev][2] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dx = GENERAL;
                                        break;
                                default:
                                        break;
                                }
                                switch (sys_data.tls[dev].fault.dy) {
                                case NORMAL:
                                        if (delta[dev][1] > cfg.dy[dev][2] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dy = SERIOUS;
                                        else if (delta[dev][1] > cfg.dy[dev][1] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dy = GENERAL;
                                        else if (delta[dev][1] > cfg.dy[dev][0] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dy = WARNING;
                                        break;
                                case WARNING:
                                        if (delta[dev][1] > cfg.dy[dev][2] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dy = SERIOUS;
                                        else if (delta[dev][1] > cfg.dy[dev][1] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dy = GENERAL;
                                        else if (delta[dev][1] < -100 + cfg.dy[dev][0] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dy = NORMAL;
                                        break;
                                case GENERAL:
                                        if (delta[dev][1] > cfg.dy[dev][2] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dy = SERIOUS;
                                        else if (delta[dev][1] < -100 + cfg.dy[dev][0] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dy = NORMAL;
                                        else if (delta[dev][1] < -100 + cfg.dy[dev][1] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dy = WARNING;
                                        break;
                                case SERIOUS:
                                        if (delta[dev][1] < -100 + cfg.dy[dev][0] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dy = NORMAL;
                                        else if (delta[dev][1] < -100 + cfg.dy[dev][1] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dy = WARNING;
                                        else if (delta[dev][1] < -100 + cfg.dy[dev][2] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dy = GENERAL;
                                        break;
                                default:
                                        break;
                                }
                                switch (err[dev]) {
                                case 0x00:
                                        sys_data.tls[dev].fault.dev = NORMAL;
                                        break;
                                case 0x0c:
                                        sys_data.tls[dev].fault.dev = 1;
                                        break;
                                case 0xf0:
                                        sys_data.tls[dev].fault.dev = 2;
                                        break;
                                case 0x03:
                                        sys_data.tls[dev].fault.dev = 3;
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
                                        if (p[0]->can && p[1]->can) {
                                                xd = abs(cur[0][0] - cur[1][0]);
                                                yd = abs(cur[0][1] - cur[1][1]);
                                        } else {
                                                xd = 0;
                                                yd = 0;
                                        }
                                        for (i = 0; i < DEVS; i++) {
                                                switch (sys_data.tls[i].fault.xdiff) {
                                                case NORMAL:
                                                        if (xd > cfg.xd[2] + relax[dev][XDIFF])
                                                                sys_data.tls[i].fault.xdiff = SERIOUS;
                                                        else if (xd > cfg.xd[1] + relax[dev][XDIFF])
                                                                sys_data.tls[i].fault.xdiff = GENERAL;
                                                        else if (xd > cfg.xd[0] + relax[dev][XDIFF])
                                                                sys_data.tls[i].fault.xdiff = WARNING;
                                                        break;
                                                case WARNING:
                                                        if (xd > cfg.xd[2] + relax[dev][XDIFF])
                                                                sys_data.tls[i].fault.xdiff = SERIOUS;
                                                        else if (xd > cfg.xd[1] + relax[dev][XDIFF])
                                                                sys_data.tls[i].fault.xdiff = GENERAL;
                                                        else if (xd < -100 + cfg.xd[0] + relax[dev][XDIFF])
                                                                sys_data.tls[i].fault.xdiff = NORMAL;
                                                        break;
                                                case GENERAL:
                                                        if (xd > cfg.xd[2] + relax[dev][XDIFF])
                                                                sys_data.tls[i].fault.xdiff = SERIOUS;
                                                        else if (xd < -100 + cfg.xd[0] + relax[dev][XDIFF])
                                                                sys_data.tls[i].fault.xdiff = NORMAL;
                                                        else if (xd < -100 + cfg.xd[1] + relax[dev][XDIFF])
                                                                sys_data.tls[i].fault.xdiff = WARNING;
                                                        break;
                                                case SERIOUS:
                                                        if (xd < -100 + cfg.xd[0] + relax[dev][XDIFF])
                                                                sys_data.tls[i].fault.xdiff = NORMAL;
                                                        else if (xd < -100 + cfg.xd[1] + relax[dev][XDIFF])
                                                                sys_data.tls[i].fault.xdiff = WARNING;
                                                        else if (xd < -100 + cfg.xd[2] + relax[dev][XDIFF])
                                                                sys_data.tls[i].fault.xdiff = GENERAL;
                                                        break;
                                                default:
                                                        break;
                                                }
                                                switch (sys_data.tls[i].fault.ydiff) {
                                                case NORMAL:
                                                        if (yd > cfg.yd[2] + relax[dev][YDIFF])
                                                                sys_data.tls[i].fault.ydiff = SERIOUS;
                                                        else if (yd > cfg.yd[1] + relax[dev][YDIFF])
                                                                sys_data.tls[i].fault.ydiff = GENERAL;
                                                        else if (yd > cfg.yd[0] + relax[dev][YDIFF])
                                                                sys_data.tls[i].fault.ydiff = WARNING;
                                                        break;
                                                case WARNING:
                                                        if (yd > cfg.yd[2] + relax[dev][YDIFF])
                                                                sys_data.tls[i].fault.ydiff = SERIOUS;
                                                        else if (yd > cfg.yd[1] + relax[dev][YDIFF])
                                                                sys_data.tls[i].fault.ydiff = GENERAL;
                                                        else if (yd < -100 + cfg.yd[0] + relax[dev][YDIFF])
                                                                sys_data.tls[i].fault.ydiff = NORMAL;
                                                        break;
                                                case GENERAL:
                                                        if (yd > cfg.yd[2] + relax[dev][YDIFF])
                                                                sys_data.tls[i].fault.ydiff = SERIOUS;
                                                        else if (yd < -100 + cfg.yd[0] + relax[dev][YDIFF])
                                                                sys_data.tls[i].fault.ydiff = NORMAL;
                                                        else if (yd < -100 + cfg.yd[1] + relax[dev][YDIFF])
                                                                sys_data.tls[i].fault.ydiff = WARNING;
                                                        break;
                                                case SERIOUS:
                                                        if (yd < -100 + cfg.yd[0] + relax[dev][YDIFF])
                                                                sys_data.tls[i].fault.ydiff = NORMAL;
                                                        else if (yd < -100 + cfg.yd[1] + relax[dev][YDIFF])
                                                                sys_data.tls[i].fault.ydiff = WARNING;
                                                        else if (yd < -100 + cfg.yd[2] + relax[dev][YDIFF])
                                                                sys_data.tls[i].fault.ydiff = GENERAL;
                                                        break;
                                                default:
                                                        break;
                                                }
                                        }
                                } else {
                                        sys_data.tls[0].fault.xdiff = NORMAL;
                                        sys_data.tls[0].fault.ydiff = NORMAL;
                                        sys_data.tls[1].fault.xdiff = NORMAL;
                                        sys_data.tls[1].fault.ydiff = NORMAL;
                                }
                                sys_data.tls[dev].x = cur[dev][0];
                                sys_data.tls[dev].y = cur[dev][1];
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
                                        switch (sys_data.tls[i].fault.link) {
                                        case NORMAL:
                                                if (link[i][NO] > 2 + relax[i][LINK])
                                                        sys_data.tls[i].fault.link = SERIOUS;
                                                else if (link[i][NO] > 1 + relax[i][LINK])
                                                        sys_data.tls[i].fault.link = GENERAL;
                                                else if (link[i][NO] > 0 + relax[i][LINK])
                                                        sys_data.tls[i].fault.link = WARNING;
                                                break;
                                        case WARNING:
                                                if (link[i][NO] > 2 + relax[i][LINK])
                                                        sys_data.tls[i].fault.link = SERIOUS;
                                                else if (link[i][NO] > 1 + relax[i][LINK])
                                                        sys_data.tls[i].fault.link = GENERAL;
                                                else if (link[i][YES] > 9 - relax[i][LINK])
                                                        sys_data.tls[i].fault.link = NORMAL;
                                                break;
                                        case GENERAL:
                                                if (link[i][NO] > 2 + relax[i][LINK])
                                                        sys_data.tls[i].fault.link = SERIOUS;
                                                else if (link[i][YES] > 9 - relax[i][LINK])
                                                        sys_data.tls[i].fault.link = NORMAL;
                                                else if (link[i][YES] > 8 - relax[i][LINK])
                                                        sys_data.tls[i].fault.link = WARNING;
                                                break;
                                        case SERIOUS:
                                                if (link[i][YES] > 9 - relax[i][LINK])
                                                        sys_data.tls[i].fault.link = NORMAL;
                                                else if (link[i][YES] > 8 - relax[i][LINK])
                                                        sys_data.tls[i].fault.link = WARNING;
                                                else if (link[i][YES] > 7 - relax[i][LINK])
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
