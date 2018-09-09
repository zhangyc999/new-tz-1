#ifndef UDP_H_
#define UDP_H_

#include "type.h"
#include "vx.h"

#define SERVER_ADDRESS "192.168.100.130"
#define GROUP_ADDRESS "234.1.1.9"
#define SERVER_PORT 4207
#define CLIENT_PORT 4201

typedef struct udp_rx {
        NODE node;
        u32 head;
        u32 cmd[2];
        u32 ts;
} UDP_RX;

typedef struct udp_tx {
        u32 head;
        u32 res0;
        struct {
                u8 id[4];
                u8 data[8];
                u32 ts;
        } can[50];
        u32 flag[50];
        u32 res1[4];
        u32 ts;
} UDP_TX;

#endif /* UDP_H_ */
