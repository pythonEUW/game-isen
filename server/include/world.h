#ifndef WORLD_H
#define WORLD_H

#include "player.h"
#include "map.h"
#include "protocol.h"

typedef struct {
    GameMap map;
    Player players[MAX_PLAYERS];
    uint32_t next_player_id;
} World;

void world_init(World *world);
Player *world_add_player(World *world);
Player *world_find_player(World *world, uint32_t id);
void world_remove_player(World *world, uint32_t id);

void world_update(World *world, float dt);

#endif
