
#include "world.h"

#include <stddef.h>

void world_init(World *world)
{
    *world = (World){0};
    world->next_player_id = 1;
}

Player *world_add_player(World *world)
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!world->players[i].active) {
            Player *player = &world->players[i];

            player_init(player,
                        world->next_player_id++,
                        100.0f, 100.0f);

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
        player_update(&world->players[i], dt);
    }
}
