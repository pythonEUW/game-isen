#ifndef GAME_RULES_H
#define GAME_RULES_H

#include <stdint.h>
#include <raymath.h>

/* Carte */
#define MAP_WIDTH        2000.0f
#define MAP_HEIGHT       2000.0f

/* Joueur */
#define PLAYER_SPEED     200.0f
#define PLAYER_MAX_HP    100
#define PLAYER_RADIUS    16.0f

/* Simulation serveur */
#define SERVER_TICK_RATE 30 

typedef struct {
    float x, y;
} Vec2;

typedef struct {
    uint8_t id;
    Vector2 pos;
    int health;
    bool active; //défini si le joueur peux bouger ou non
} PlayerState;

#endif
