#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include "game_rules.h"

#define GAME_PORT          7777
#define MAX_PLAYERS        4
#define ENET_CHANNEL_COUNT 2
#define CHANNEL_RELIABLE   0
#define CHANNEL_UNRELIABLE 1

typedef enum {
    MSG_INPUT = 1,
    MSG_WELCOME,
    MSG_WORLD_STATE,
} MessageType;

typedef struct {
    int up, down, left, right;
} PlayerInput;

typedef struct { uint8_t type; PlayerInput input; } MsgInput;
typedef struct { uint8_t type; uint8_t player_id; } MsgWelcome;
typedef struct {
    uint8_t     type;
    uint8_t     count;
    PlayerState players[MAX_PLAYERS];
} MsgWorldState;

#endif
