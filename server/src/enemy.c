#include "enemy.h"

#include "game_config.h"

void enemy_init(Enemy *enemy, uint32_t id)
{
    *enemy = (Enemy){
        .id = id,
        .health = ENEMY_HEALTH,
        .speed = ENEMY_SPEED,
        .active = true
    };
}
