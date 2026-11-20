#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_SPELLS 5

typedef struct {
    uint32_t id;

    Vector2 pos;

    uint32_t health;
    uint32_t level;
    uint32_t xp;

    uint32_t spells_lists[MAX_SPELLS];

    float speed;

    bool active;

    int8_t input_x;
    int8_t input_y;

} Player;

#endif
