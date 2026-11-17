#ifndef ENEMY_H
#define ENEMY_H

#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t id;
    Vector2 pos;
    uint32_t health;
    float speed;
    bool active;
} Enemy;

#endif
