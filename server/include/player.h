#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include <raylib.h>
#include <stdint.h>

#include "../../shared/include/player.h"

void player_init(Player *player, uint32_t id);

void player_update(Player *player);

#endif
