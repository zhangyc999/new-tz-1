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

#define DEVS 2

#define LINK 0
#define PMIN 1
#define PMAX 2
#define RMIN 3
#define RMAX 4
#define DP   5
#define DR   6
#define PD   7
#define RD   8

void t_tls(int period, int duration)
{
        struct {
                int tid;
                struct ext *p;
        } send;
        struct {
                int pofs[DEVS];
                int rofs[DEVS];
                int pmin[DEVS][3];
                int pmax[DEVS][3];
                int rmin[DEVS][3];
                int rmax[DEVS][3];
                int dp[DEVS][3];
                int dr[DEVS][3];
                int pd[3];
                int rd[3];
        } cfg;
        int delay = period;
        int stamp;
        int tmp[DEVS];
        int ctr[DEVS] = {0, 0};
        int link[DEVS][2] = {{0}, {0}};
        int psum[DEVS] = {0};
        int rsum[DEVS] = {0};
        int pavg[DEVS] = {0};
        int ravg[DEVS] = {0};
        int pcur[DEVS] = {0};
        int rcur[DEVS] = {0};
        int err[DEVS] = {0};
        int dp[DEVS] = {0};
        int dr[DEVS] = {0};
        int pd = 0;
        int rd = 0;
        int relax[DEVS][16] = {{0}, {0}};
        int i, j;
        int fd = open("/tls.cfg", O_RDWR, 0644);
        unsigned char dev;
        CMD *cmd;
        struct ext can[DEVS];
        struct can buf[DEVS][duration];
        struct can *p[DEVS];
        struct can *prev[DEVS];
        LIST lst[DEVS];
        memset(buf, 0, sizeof(buf));
        cfg.pofs[0] = 0;
        cfg.pofs[1] = 0;
        cfg.rofs[0] = 0;
        cfg.rofs[1] = 0;
        cfg.pmin[0][0] = -500;
        cfg.pmax[0][0] = +500;
        cfg.pmin[0][1] = -1000;
        cfg.pmax[0][1] = +1000;
        cfg.pmin[0][2] = -2000;
        cfg.pmax[0][2] = +2000;
        cfg.pmin[1][0] = -500;
        cfg.pmax[1][0] = +500;
        cfg.pmin[1][1] = -1000;
        cfg.pmax[1][1] = +1000;
        cfg.pmin[1][2] = -2000;
        cfg.pmax[1][2] = +2000;
        cfg.rmin[0][0] = -500;
        cfg.rmax[0][0] = +500;
        cfg.rmin[0][1] = -1000;
        cfg.rmax[0][1] = +1000;
        cfg.rmin[0][2] = -2000;
        cfg.rmax[0][2] = +2000;
        cfg.rmin[1][0] = -500;
        cfg.rmax[1][0] = +500;
        cfg.rmin[1][1] = -1000;
        cfg.rmax[1][1] = +1000;
        cfg.rmin[1][2] = -2000;
        cfg.rmax[1][2] = +2000;
        cfg.dp[0][0] = 200;
        cfg.dp[0][1] = 500;
        cfg.dp[0][2] = 1000;
        cfg.dr[1][0] = 200;
        cfg.dr[1][1] = 500;
        cfg.dr[1][2] = 1000;
        cfg.pd[0] = 200;
        cfg.pd[1] = 500;
        cfg.pd[2] = 1000;
        cfg.rd[0] = 200;
        cfg.rd[1] = 500;
        cfg.rd[2] = 1000;
        if (fd)
                read(fd, (char *)&cfg, sizeof(cfg));
        for (i = 0; i < DEVS; i++) {
                lstInit(&lst[i]);
                for (j = 0; j < duration; j++)
                        lstAdd(&lst[i], (NODE *)&buf[i][j]);
                lstFirst(&lst[i])->previous = lstLast(&lst[i]);
                lstLast(&lst[i])->next = lstFirst(&lst[i]);
                p[i] = (struct can *)lstFirst(&lst[i]);
        }
        send.tid = taskIdSelf();
        for (;;) {
                stamp = tickGet();
                if (delay < 0 || delay > period)
                        delay = 0;
                if (8 == msgQReceive(msg_tls, (char *)tmp, 8, delay)) {
                        if (tmp[0] == tid_core) {
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
                                                        switch (cmd->data.tls.relax.pmin) {
                                                        case 0:
                                                                relax[i][PMIN] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][PMIN] = -100;
                                                                break;
                                                        case 2:
                                                                relax[i][PMIN] = -300;
                                                                break;
                                                        case 3:
                                                                relax[i][PMIN] = -1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.pmax) {
                                                        case 0:
                                                                relax[i][PMAX] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][PMAX] = +100;
                                                                break;
                                                        case 2:
                                                                relax[i][PMAX] = +300;
                                                                break;
                                                        case 3:
                                                                relax[i][PMAX] = +1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.rmin) {
                                                        case 0:
                                                                relax[i][RMIN] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][RMIN] = -100;
                                                                break;
                                                        case 2:
                                                                relax[i][RMIN] = -300;
                                                                break;
                                                        case 3:
                                                                relax[i][RMIN] = -1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.rmax) {
                                                        case 0:
                                                                relax[i][RMAX] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][RMAX] = +100;
                                                                break;
                                                        case 2:
                                                                relax[i][RMAX] = +300;
                                                                break;
                                                        case 3:
                                                                relax[i][RMAX] = +1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.dp) {
                                                        case 0:
                                                                relax[i][DP] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][DP] = 100;
                                                                break;
                                                        case 2:
                                                                relax[i][DP] = 300;
                                                                break;
                                                        case 3:
                                                                relax[i][DP] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.dr) {
                                                        case 0:
                                                                relax[i][DR] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][DR] = 100;
                                                                break;
                                                        case 2:
                                                                relax[i][DR] = 300;
                                                                break;
                                                        case 3:
                                                                relax[i][DR] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.pd) {
                                                        case 0:
                                                                relax[i][PD] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][PD] = 100;
                                                                break;
                                                        case 2:
                                                                relax[i][PD] = 300;
                                                                break;
                                                        case 3:
                                                                relax[i][PD] = 1000;
                                                                break;
                                                        default:
                                                                break;
                                                        }
                                                        switch (cmd->data.tls.relax.rd) {
                                                        case 0:
                                                                relax[i][RD] = 0;
                                                                break;
                                                        case 1:
                                                                relax[i][RD] = 100;
                                                                break;
                                                        case 2:
                                                                relax[i][RD] = 300;
                                                                break;
                                                        case 3:
                                                                relax[i][RD] = 1000;
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
                                switch (((struct ext *)tmp[1])->id[0]) {
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
                                        psum[dev] -= *(short *)&p[dev]->can->data[0];
                                        rsum[dev] -= *(short *)&p[dev]->can->data[2];
                                        ctr[dev]--;
                                }
                                p[dev]->can = (struct ext *)tmp[1];
                                pcur[dev] = *(short *)&p[dev]->can->data[0] + cfg.pofs[dev];
                                rcur[dev] = *(short *)&p[dev]->can->data[2] + cfg.rofs[dev];
                                err[dev] = *(unsigned char *)&p[dev]->can->data[6];
                                psum[dev] += *(short *)&p[dev]->can->data[0];
                                rsum[dev] += *(short *)&p[dev]->can->data[2];
                                ctr[dev]++;
                                pavg[dev] = psum[dev] / ctr[dev] + cfg.pofs[dev];
                                ravg[dev] = rsum[dev] / ctr[dev] + cfg.rofs[dev];
                                prev[dev] = (struct can *)lstPrevious((NODE *)p[dev]);
                                if (prev[dev]->can) {
                                        dp[dev] = abs(pcur[dev] - * (short *)&prev[dev]->can->data[0]);
                                        dr[dev] = abs(rcur[dev] - * (short *)&prev[dev]->can->data[2]);
                                } else {
                                        dp[dev] = 0;
                                        dr[dev] = 0;
                                }
                                switch (sys_data.tls[dev].fault.x) {
                                case NORMAL:
                                        if (pcur[dev] > +100 + cfg.x[dev][2] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = SERIOUS;
                                        else if (pcur[dev] > +100 + cfg.x[dev][1] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = GENERAL;
                                        else if (pcur[dev] > +100 + cfg.x[dev][0] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = WARNING;
                                        break;
                                case WARNING:
                                        if (pcur[dev] > +100 + cfg.x[dev][2] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = SERIOUS;
                                        else if (pcur[dev] > +100 + cfg.x[dev][1] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = GENERAL;
                                        else if (pcur[dev] < -100 + cfg.x[dev][0] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = NORMAL;
                                        break;
                                case GENERAL:
                                        if (pcur[dev] > +100 + cfg.x[dev][2] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = SERIOUS;
                                        else if (pcur[dev] < -100 + cfg.x[dev][0] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = NORMAL;
                                        else if (pcur[dev] < -100 + cfg.x[dev][1] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = WARNING;
                                        break;
                                case SERIOUS:
                                        if (pcur[dev] < -100 + cfg.x[dev][0] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = NORMAL;
                                        else if (pcur[dev] < -100 + cfg.x[dev][1] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = WARNING;
                                        else if (pcur[dev] < -100 + cfg.x[dev][2] + relax[dev][X])
                                                sys_data.tls[dev].fault.x = GENERAL;
                                        break;
                                default:
                                        break;
                                }
                                switch (sys_data.tls[dev].fault.y) {
                                case NORMAL:
                                        if (rcur[dev] > +100 + cfg.y[dev][2] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = SERIOUS;
                                        else if (rcur[dev] > +100 + cfg.y[dev][1] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = GENERAL;
                                        else if (rcur[dev] > +100 + cfg.y[dev][0] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = WARNING;
                                        break;
                                case WARNING:
                                        if (rcur[dev] > +100 + cfg.y[dev][2] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = SERIOUS;
                                        else if (rcur[dev] > +100 + cfg.y[dev][1] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = GENERAL;
                                        else if (rcur[dev] < -100 + cfg.y[dev][0] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = NORMAL;
                                        break;
                                case GENERAL:
                                        if (rcur[dev] > +100 + cfg.y[dev][2] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = SERIOUS;
                                        else if (rcur[dev] < -100 + cfg.y[dev][0] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = NORMAL;
                                        else if (rcur[dev] < -100 + cfg.y[dev][1] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = WARNING;
                                        break;
                                case SERIOUS:
                                        if (rcur[dev] < -100 + cfg.y[dev][0] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = NORMAL;
                                        else if (rcur[dev] < -100 + cfg.y[dev][1] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = WARNING;
                                        else if (rcur[dev] < -100 + cfg.y[dev][2] + relax[dev][Y])
                                                sys_data.tls[dev].fault.y = GENERAL;
                                        break;
                                default:
                                        break;
                                }
                                switch (sys_data.tls[dev].fault.dp) {
                                case NORMAL:
                                        if (dp[dev] > +100 + cfg.dp[dev][2] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dp = SERIOUS;
                                        else if (dp[dev] > +100 + cfg.dp[dev][1] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dp = GENERAL;
                                        else if (dp[dev] > +100 + cfg.dp[dev][0] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dp = WARNING;
                                        break;
                                case WARNING:
                                        if (dp[dev] > +100 + cfg.dp[dev][2] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dp = SERIOUS;
                                        else if (dp[dev] > +100 + cfg.dp[dev][1] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dp = GENERAL;
                                        else if (dp[dev] < -100 + cfg.dp[dev][0] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dp = NORMAL;
                                        break;
                                case GENERAL:
                                        if (dp[dev] > +100 + cfg.dp[dev][2] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dp = SERIOUS;
                                        else if (dp[dev] < -100 + cfg.dp[dev][0] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dp = NORMAL;
                                        else if (dp[dev] < -100 + cfg.dp[dev][1] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dp = WARNING;
                                        break;
                                case SERIOUS:
                                        if (dp[dev] < -100 + cfg.dp[dev][0] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dp = NORMAL;
                                        else if (dp[dev] < -100 + cfg.dp[dev][1] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dp = WARNING;
                                        else if (dp[dev] < -100 + cfg.dp[dev][2] + relax[dev][DX])
                                                sys_data.tls[dev].fault.dp = GENERAL;
                                        break;
                                default:
                                        break;
                                }
                                switch (sys_data.tls[dev].fault.dr) {
                                case NORMAL:
                                        if (dr[dev] > +100 + cfg.dr[dev][2] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dr = SERIOUS;
                                        else if (dr[dev] > +100 + cfg.dr[dev][1] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dr = GENERAL;
                                        else if (dr[dev] > +100 + cfg.dr[dev][0] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dr = WARNING;
                                        break;
                                case WARNING:
                                        if (dr[dev] > +100 + cfg.dr[dev][2] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dr = SERIOUS;
                                        else if (dr[dev] > +100 + cfg.dr[dev][1] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dr = GENERAL;
                                        else if (dr[dev] < -100 + cfg.dr[dev][0] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dr = NORMAL;
                                        break;
                                case GENERAL:
                                        if (dr[dev] > +100 + cfg.dr[dev][2] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dr = SERIOUS;
                                        else if (dr[dev] < -100 + cfg.dr[dev][0] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dr = NORMAL;
                                        else if (dr[dev] < -100 + cfg.dr[dev][1] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dr = WARNING;
                                        break;
                                case SERIOUS:
                                        if (dr[dev] < -100 + cfg.dr[dev][0] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dr = NORMAL;
                                        else if (dr[dev] < -100 + cfg.dr[dev][1] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dr = WARNING;
                                        else if (dr[dev] < -100 + cfg.dr[dev][2] + relax[dev][DY])
                                                sys_data.tls[dev].fault.dr = GENERAL;
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
                                    sys_data.tls[0].fault.dp < GENERAL && sys_data.tls[1].fault.dp < GENERAL &&
                                    sys_data.tls[0].fault.dr < GENERAL && sys_data.tls[1].fault.dr < GENERAL &&
                                    sys_data.tls[0].fault.dev == NORMAL && sys_data.tls[1].fault.dev == NORMAL) {
                                        if (p[0]->can && p[1]->can) {
                                                pd = abs(pcur[0] - pcur[1]);
                                                rd = abs(rcur[0] - rcur[1]);
                                        } else {
                                                pd = 0;
                                                rd = 0;
                                        }
                                        for (i = 0; i < DEVS; i++) {
                                                switch (sys_data.tls[i].fault.pd) {
                                                case NORMAL:
                                                        if (pd > +100 + cfg.pd[2] + relax[dev][XD])
                                                                sys_data.tls[i].fault.pd = SERIOUS;
                                                        else if (pd > +100 + cfg.pd[1] + relax[dev][XD])
                                                                sys_data.tls[i].fault.pd = GENERAL;
                                                        else if (pd > +100 + cfg.pd[0] + relax[dev][XD])
                                                                sys_data.tls[i].fault.pd = WARNING;
                                                        break;
                                                case WARNING:
                                                        if (pd > +100 + cfg.pd[2] + relax[dev][XD])
                                                                sys_data.tls[i].fault.pd = SERIOUS;
                                                        else if (pd > +100 + cfg.pd[1] + relax[dev][XD])
                                                                sys_data.tls[i].fault.pd = GENERAL;
                                                        else if (pd < -100 + cfg.pd[0] + relax[dev][XD])
                                                                sys_data.tls[i].fault.pd = NORMAL;
                                                        break;
                                                case GENERAL:
                                                        if (pd > +100 + cfg.pd[2] + relax[dev][XD])
                                                                sys_data.tls[i].fault.pd = SERIOUS;
                                                        else if (pd < -100 + cfg.pd[0] + relax[dev][XD])
                                                                sys_data.tls[i].fault.pd = NORMAL;
                                                        else if (pd < -100 + cfg.pd[1] + relax[dev][XD])
                                                                sys_data.tls[i].fault.pd = WARNING;
                                                        break;
                                                case SERIOUS:
                                                        if (pd < -100 + cfg.pd[0] + relax[dev][XD])
                                                                sys_data.tls[i].fault.pd = NORMAL;
                                                        else if (pd < -100 + cfg.pd[1] + relax[dev][XD])
                                                                sys_data.tls[i].fault.pd = WARNING;
                                                        else if (pd < -100 + cfg.pd[2] + relax[dev][XD])
                                                                sys_data.tls[i].fault.pd = GENERAL;
                                                        break;
                                                default:
                                                        break;
                                                }
                                                switch (sys_data.tls[i].fault.rd) {
                                                case NORMAL:
                                                        if (rd > +100 + cfg.rd[2] + relax[dev][YD])
                                                                sys_data.tls[i].fault.rd = SERIOUS;
                                                        else if (rd > +100 + cfg.rd[1] + relax[dev][YD])
                                                                sys_data.tls[i].fault.rd = GENERAL;
                                                        else if (rd > +100 + cfg.rd[0] + relax[dev][YD])
                                                                sys_data.tls[i].fault.rd = WARNING;
                                                        break;
                                                case WARNING:
                                                        if (rd > +100 + cfg.rd[2] + relax[dev][YD])
                                                                sys_data.tls[i].fault.rd = SERIOUS;
                                                        else if (rd > +100 + cfg.rd[1] + relax[dev][YD])
                                                                sys_data.tls[i].fault.rd = GENERAL;
                                                        else if (rd < -100 + cfg.rd[0] + relax[dev][YD])
                                                                sys_data.tls[i].fault.rd = NORMAL;
                                                        break;
                                                case GENERAL:
                                                        if (rd > +100 + cfg.rd[2] + relax[dev][YD])
                                                                sys_data.tls[i].fault.rd = SERIOUS;
                                                        else if (rd < -100 + cfg.rd[0] + relax[dev][YD])
                                                                sys_data.tls[i].fault.rd = NORMAL;
                                                        else if (rd < -100 + cfg.rd[1] + relax[dev][YD])
                                                                sys_data.tls[i].fault.rd = WARNING;
                                                        break;
                                                case SERIOUS:
                                                        if (rd < -100 + cfg.rd[0] + relax[dev][YD])
                                                                sys_data.tls[i].fault.rd = NORMAL;
                                                        else if (rd < -100 + cfg.rd[1] + relax[dev][YD])
                                                                sys_data.tls[i].fault.rd = WARNING;
                                                        else if (rd < -100 + cfg.rd[2] + relax[dev][YD])
                                                                sys_data.tls[i].fault.rd = GENERAL;
                                                        break;
                                                default:
                                                        break;
                                                }
                                        }
                                } else {
                                        sys_data.tls[0].fault.pd = NORMAL;
                                        sys_data.tls[0].fault.rd = NORMAL;
                                        sys_data.tls[1].fault.pd = NORMAL;
                                        sys_data.tls[1].fault.rd = NORMAL;
                                }
                                sys_data.tls[dev].x = pcur[dev];
                                sys_data.tls[dev].y = rcur[dev];
                                p[dev] = (struct can *)lstNext((NODE *)p[dev]);
                                delay -= tickGet() - stamp;
                        } else {
                                for (i = 0; i < 2; i++) {
                                        prev[i] = (struct can *)lstPrevious((NODE *)p[i]);
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
                                msgQSend(msg_can[0][0], (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
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
                                msgQSend(msg_can[1][0], (char *)&send, 8, NO_WAIT, MSG_PRI_NORMAL);
                                delay = period;
                        }
                }
        }
}
