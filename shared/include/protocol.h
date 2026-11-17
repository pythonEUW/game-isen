#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define GAME_PORT 7777
#define MAX_PLAYERS 4
#define ENET_CHANNEL_COUNT 2

typedef enum {
    PACKET_PLAYER_STATE = 1,
    PACKET_PLAYER_INPUT = 2,
    PACKET_WELCOME = 3,
    PACKET_LEVEL_UP = 4,       /* serveur -> client : 3 cartes a choisir */
    PACKET_UPGRADE_CHOICE = 5  /* client -> serveur : index de la carte choisie */
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

typedef struct {
    uint8_t type;
    uint32_t id;
} WelcomePacket;

/* ==========================================
   Level up / ameliorations
   ========================================== */

#define UPGRADE_CHOICES 3

typedef enum {
    UPGRADE_NEW_WEAPON = 0,
    UPGRADE_MAX_HEALTH,
    UPGRADE_SPEED,
    UPGRADE_DAMAGE,
    UPGRADE_ATTACK_SPEED,
    UPGRADE_COUNT
} UpgradeId;

typedef struct {
    uint8_t type;                     /* PACKET_LEVEL_UP */
    uint8_t level;                    /* nouveau niveau du joueur */
    uint8_t choices[UPGRADE_CHOICES]; /* des UpgradeId, tous differents */
} LevelUpPacket;

typedef struct {
    uint8_t type;  /* PACKET_UPGRADE_CHOICE */
    uint8_t index; /* 0 .. UPGRADE_CHOICES-1, index dans LevelUpPacket.choices */
} UpgradeChoicePacket;

/* Nom et description courte d'une amelioration (textes communs client/serveur). */
const char *upgrade_name(UpgradeId id);
const char *upgrade_description(UpgradeId id);

/* Tire UPGRADE_CHOICES ameliorations differentes (utilise rand()). */
void upgrade_roll_choices(uint8_t out[UPGRADE_CHOICES]);

#endif
