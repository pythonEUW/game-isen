
#include "world.h"

#include "game_config.h"

#include <raylib.h>
#include <stddef.h>
#include <math.h>

void world_init(World *world)
{
    *world = (World){0};
    world->next_player_id = 1;
    world->next_enemy_id = 1;
    map_init(&world->map, MAP_DEFAULT_SEED);
    world_add_enemy(world);
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
            player->pos = (Vector2){
                PLAYER_SPAWN_X + i * PLAYER_SPAWN_SPACING,
                PLAYER_SPAWN_Y
            };
            player->speed = PLAYER_SPEED;
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

Enemy *world_add_enemy(World *world)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!world->enemies[i].active) {
            Enemy *enemy = &world->enemies[i];
            enemy_init(enemy, world->next_enemy_id++);
            enemy->pos = (Vector2){ENEMY_SPAWN_X, ENEMY_SPAWN_Y};
            return enemy;
        }
    }

    return NULL;
}

Enemy *world_find_enemy(World *world, uint32_t id)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy *enemy = &world->enemies[i];

        if (enemy->active && enemy->id == id)
            return enemy;
    }

    return NULL;
}

void world_remove_enemy(World *world, uint32_t id)
{
    Enemy *enemy = world_find_enemy(world, id);

    if (enemy)
        enemy->active = false;
}

static Player *world_find_closest_player(World *world, Vector2 position)
{
    Player *closest = NULL;
    float closest_distance = 0.0f;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player *player = &world->players[i];
        if (!player->active)
            continue;

        float dx = player->pos.x - position.x;
        float dy = player->pos.y - position.y;
        float distance = dx * dx + dy * dy;

        if (closest == NULL || distance < closest_distance) {
            closest = player;
            closest_distance = distance;
        }
    }

    return closest;
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

    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy *enemy = &world->enemies[i];
        if (!enemy->active)
            continue;

        Player *target = world_find_closest_player(world, enemy->pos);
        if (target == NULL)
            continue;

        float dx = target->pos.x - enemy->pos.x;
        float dy = target->pos.y - enemy->pos.y;
        float distance = sqrtf(dx * dx + dy * dy);
        if (!isfinite(distance) || distance <= 0.0f)
            continue;

        map_move_circle(&world->map,
                        &enemy->pos.x,
                        &enemy->pos.y,
                        dx / distance * enemy->speed * dt,
                        dy / distance * enemy->speed * dt,
                        ENEMY_RADIUS);
    }
}
