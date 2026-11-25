#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "player.h"
#include <stdint.h>

#define GAME_PORT 7777
#define MAX_PLAYERS 4
#define ENET_CHANNEL_COUNT 2

typedef enum {
    PACKET_PLAYER_STATE = 1,
    PACKET_PLAYER_INPUT = 2,
    PACKET_WELCOME = 3,
    PACKET_INVENTORY = 4,
    PACKET_LEVEL_UP = 5,
    PACKET_SPELL_CHOICE = 6
} PacketType;

typedef struct {
    uint8_t type;
    uint32_t id;
    float x;
    float y;
    int32_t health;
    int32_t level;
    int32_t xp;
    uint8_t active;
} PlayerStatePacket;

typedef struct {
    uint8_t type;
    int8_t input_x;
    int8_t input_y;
} PlayerInputPacket;

typedef struct {
    uint8_t type;
    uint32_t id;
} WelcomePacket;

typedef struct {
    uint8_t type;
    uint32_t player_id;
    uint32_t new_level;
} LevelUpPacket;


typedef struct {
    uint32_t spell_id;
    uint32_t level;
} InventorySlot;

typedef struct {
    uint8_t type;
    uint32_t player_id;
    uint32_t count;
    InventorySlot slots[MAX_SPELLS];
} InventoryPacket;

typedef struct {
    uint8_t type;
    uint32_t player_id;
    uint32_t spells[3];
} SpellChoicePacket;

#endif
