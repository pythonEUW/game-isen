#include "enemy.h"

#include "game_config.h"

void enemy_init(Enemy *enemy, uint32_t id, EnemyType type)
{
    *enemy = (Enemy){0};
    enemy->id = id;
    enemy->type = type;
    enemy->level = 1;
    enemy->active = true;

    switch (type) {
        case ENEMY_TYPE_BAT:
            enemy->speed = 120.0f;
            enemy->health = 40;
            break;
        case ENEMY_TYPE_MINOTAUR:
            enemy->speed = 50.0f;
            enemy->health = 240;
            break;
        case ENEMY_TYPE_SKELETON:
        default:
            enemy->speed = 80.0f;
            enemy->health = 100;
            break;
    }
}
