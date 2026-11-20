#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include <raylib.h>
#include <stdint.h>

#include "../../shared/include/player.h"
#include "spell.h"

void player_init(Player *player, uint32_t id);

void player_update(Player *player);

long long xp_needed_for_level(int level);

long long xp_to_next_level(Player *player);

void spell_selection(uint32_t *out, size_t max, size_t *count);

#endif
