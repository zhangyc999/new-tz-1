#ifndef UDP_H_
#define UDP_H_

#include "cmd.h"
#include "type.h"
#include "vx.h"

typedef struct {
        NODE node;
        u32 ts;
        u16 head;
        u16 len;
        CMD cmd;
} UDP;

#define SERVER_ADDRESS "192.168.100.130"
#define GROUP_ADDRESS "234.1.1.9"
#define SERVER_PORT 4207
#define CLIENT_PORT 4201

#endif /* UDP_H_ */
