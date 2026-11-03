
#include "player.h"

#include <math.h>

void player_init(Player *player, uint32_t id,
                 float x, float y)
{
    *player = (Player){
        .id = id,
        .x = x,
        .y = y,
        .speed = 200.0f,
        .health = 100,
        .active = true,
        .input_x = 0.0f,
        .input_y = 0.0f
    };
}

void player_update(Player *player, float dt)
{
    if (!player->active)
        return;

    float dx = player->input_x;
    float dy = player->input_y;

    // Eviter d'aller plus vite en diagonale
    float length = sqrtf(dx * dx + dy * dy);

    if (length > 1.0f) {
        dx /= length;
        dy /= length;
    }

    player->x += dx * player->speed * dt;
    player->y += dy * player->speed * dt;
}
