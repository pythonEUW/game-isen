
#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H

#include <enet/enet.h>
#include "world.h"

typedef struct {
    ENetHost *host;
} NetworkServer;

int network_server_init(NetworkServer *network);
int network_server_poll(NetworkServer *network, World *world);
void network_server_close(NetworkServer *network);

#endif
