
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

            // World placement and movement speed are configured outside player.c.
            player->pos = (Vector2){100.0f, 100.0f};
            player->speed = 200.0f;
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
    if (!isfinite(dt) || dt <= 0.0f)
        return;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player *player = &world->players[i];
        if (!player->active)
            continue;

        // Preserve player_update's displacement calculation. Use a copy to
        // convert speed to this frame's distance, then resolve map collisions.
        Player next = *player;
        float dx = player->input_x;
        float dy = player->input_y;
        float length = sqrtf(dx * dx + dy * dy);
        next.speed *= dt / fmaxf(1.0f, length);
        player_update(&next);
        map_move_circle(&world->map, &player->pos.x, &player->pos.y,
                        next.pos.x - player->pos.x,
                        next.pos.y - player->pos.y, PLAYER_RADIUS);
    }
}
