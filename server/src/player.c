#include "player.h"

#include <raylib.h>
#include <raymath.h>

#define BASE_XP 100

void player_init(Player *player, uint32_t id)
{
    *player = (Player){
        .id = id,
        .pos = {0.0f, 0.0f},
        .health = 100,
        .active = true,
        .level = 1,
        .xp = 0,
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

long long xp_needed_for_level(int level) {
    double d = (double)(level - 1);
    return (long long)(BASE_XP * d * d);
}

long long xp_to_next_level(Player *player) {
    return xp_needed_for_level(player->level + 1) - xp_needed_for_level(player->level);
}

void spell_selection(uint32_t *out, size_t max, size_t *count) {
    size_t n = 0;
    out[n++] = 1;
    out[n++] = 2;
    out[n++] = 3;
    *count = n;
}
