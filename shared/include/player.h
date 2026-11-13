#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t id;

    Vector2 pos;

    uint32_t health;

    float speed;

    bool active;

    int8_t input_x;
    int8_t input_y;

} Player;

#endif
