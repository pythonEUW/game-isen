#ifndef WORLD_H
#define WORLD_H

#include "enemy.h"
#include "game_config.h"
#include "player.h"
#include "map.h"
#include "protocol.h"

typedef struct {
    GameMap map;
    Player players[MAX_PLAYERS];
    Enemy enemies[MAX_ENEMIES];
    uint32_t next_player_id;
    uint32_t next_enemy_id;
} World;

void world_init(World *world);
Player *world_add_player(World *world);
Player *world_find_player(World *world, uint32_t id);
void world_remove_player(World *world, uint32_t id);
Enemy *world_add_enemy(World *world, EnemyType type);
Enemy *world_find_enemy(World *world, uint32_t id);
void world_remove_enemy(World *world, uint32_t id);

void world_update(World *world, float dt);

#endif
