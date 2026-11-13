#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define GAME_PORT 7777
#define MAX_PLAYERS 4
#define ENET_CHANNEL_COUNT 2

typedef enum {
    PACKET_PLAYER_STATE = 1
} PacketType;

typedef struct {
    uint8_t type;
    uint32_t id;
    float x;
    float y;
    int32_t health;
    uint8_t active;
} PlayerStatePacket;

#endif
