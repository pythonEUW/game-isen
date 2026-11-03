#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t id;

    float x;
    float y;

    float speed;

    int health;

    bool active;

    // Inputs reçus du client
    float input_x;
    float input_y;

} Player;

void player_init(Player *player, uint32_t id,
                 float x, float y);

void player_update(Player *player, float dt);

#endif
