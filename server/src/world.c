
#include "world.h"

#include <raylib.h>
#include <stddef.h>
#include <math.h>

void world_init(World *world)
{
    *world = (World){0};
    world->next_player_id = 1;
    map_init(&world->map, MAP_DEFAULT_SEED);
}

Player *world_add_player(World *world)
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!world->players[i].active) {
            Player *player = &world->players[i];

            player_init(player,
                        world->next_player_id++
            );

            return player;
        }
    }

    return NULL;
}

Player *world_find_player(World *world, uint32_t id)
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player *player = &world->players[i];

        if (player->active && player->id == id)
            return player;
    }

    return NULL;
}

void world_remove_player(World *world, uint32_t id)
{
    Player *player = world_find_player(world, id);

    if (player)
        player->active = false;
}

void world_update(World *world, float dt)
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player *player = &world->players[i];
        if (!player->active || !isfinite(dt) || dt <= 0.0f)
            continue;
        float dx = player->input_x;
        float dy = player->input_y;
        float length = sqrtf(dx * dx + dy * dy);
        if (length > 1.0f) {
            dx /= length;
            dy /= length;
        }
        map_move_circle(&world->map, &player->x, &player->y,
                        dx * player->speed * dt,
                        dy * player->speed * dt, PLAYER_RADIUS);
    }
}
