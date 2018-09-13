#ifndef UDP_H_
#define UDP_H_

#include "type.h"
#include "vx.h"

typedef struct udp_rx {
        NODE node;
        u32 ts;
        u32 head;
        u32 cmd[2];
} UDP_RX;

typedef struct udp_tx {
        u32 head;
        u32 res0;
        struct {
                u8 id[4];
                u8 data[8];
                u32 res;
        } can[50];
        u32 flag[50];
        u32 res1[4];
} UDP_TX;

#define SERVER_ADDRESS "192.168.100.130"
#define GROUP_ADDRESS "234.1.1.9"
#define SERVER_PORT 4207
#define CLIENT_PORT 4201

#define UMASK_UDP_ACT 0xFF00F0FF
#define UDP_ACT_IDLE  0x00000000
#define UDP_ACT_VSL   0xEC001000
#define UDP_ACT_GEND  0xEC001001
#define UDP_ACT_GENS  0xEC001002
#define UDP_ACT_PSU   0xEC001003
#define UDP_ACT_MOM   0xEC002000
#define UDP_ACT_SWH   0xEC002001
#define UDP_ACT_RSE   0xEC002002
#define UDP_ACT_SWV   0xEC002003
#define UDP_ACT_PRP   0xEC002004
#define UDP_ACT_SDT   0xEC003001
#define UDP_ACT_SDS   0xEC003002
#define UDP_ACT_SDFB  0xEC003003
#define UDP_ACT_X     0xEC004001
#define UDP_ACT_Y     0xEC004002
#define UDP_ACT_Z     0xEC004003
#define UDP_ACT_XY    0xEC004004
#define UDP_ACT_XYZ   0xEC004005

#define UMASK_UDP_MODE  0x00F00000
#define UDP_MODE_STUPID 0x00000000
#define UDP_MODE_MANUAL 0x00E00000
#define UDP_MODE_EXPERT 0x00B00000

#define UMASK_UDP_DIR 0x000F0000
#define UDP_DIR_T     0x000A0000
#define UDP_DIR_P     0x00000000
#define UDP_DIR_N     0x000C0000

#define UMASK_UDP_VSL 0x00000F00
#define UDP_VSL_GET   0x00000000
#define UDP_VSL_PUT   0x00000E00
#define UDP_VSL_JOINT 0x00000500

#define UMASK_UDP_PSU 0x00000F00
#define UDP_PSU_BRAKE 0x00000A00
#define UDP_PSU_LIGHT 0x00000C00
#define UDP_PSU_24    0x00000000
#define UDP_PSU_500   0x00000E00

#endif /* UDP_H_ */
