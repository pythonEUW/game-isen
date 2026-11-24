#ifndef ENEMY_H
#define ENEMY_H

#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    ENEMY_TYPE_SKELETON = 0,
    ENEMY_TYPE_BAT = 1,
    ENEMY_TYPE_MINOTAUR = 2
} EnemyType;

typedef struct {
    uint32_t id;
    Vector2 pos;
    uint32_t health;
    float speed;
    EnemyType type;
    uint32_t level;
    bool active;
} Enemy;

#endif
