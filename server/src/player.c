#include "player.h"

#include <raylib.h>
#include <raymath.h>

void player_init(Player *player, uint32_t id)
{
    *player = (Player){
        .id = id,
        .pos = {0.0f, 0.0f},
        .health = 100,
        .active = true,
    };
}

void player_update(Player *player)
{

    Vector2 move;

    if(player->active){
        move = (Vector2){player->input_x*player->speed, player->input_y*player->speed};
    } else {
        move = (Vector2){0.0f, 0.0f};
    }

    player->pos = Vector2Add(player->pos, move);
}
