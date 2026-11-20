#ifndef SPELL_H
#define SPELL_H

#include <raylib.h>
#include <stdint.h>
#include <uchar.h>

typedef struct {
    uint32_t id;

    char16_t name[16];

    uint32_t velocity;
    Vector2 pos;

    uint32_t health;
    uint32_t level;

    uint32_t damage;
    uint32_t damage_seconds;

    uint32_t cooldown;
} Spell;

#endif
