#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define GAME_PORT 7777
#define MAX_PLAYERS 4
#define ENET_CHANNEL_COUNT 2

typedef enum {
    PACKET_PLAYER_STATE = 1,
    PACKET_PLAYER_INPUT = 2
} PacketType;

typedef struct {
    uint8_t type;
    uint32_t id;
    float x;
    float y;
    int32_t health;
    uint8_t active;
} PlayerStatePacket;

typedef struct {
    uint8_t type;
    int8_t input_x;
    int8_t input_y;
} PlayerInputPacket;

#endif
