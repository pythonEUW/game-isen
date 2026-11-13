
#ifndef GAME_MAP_H
#define GAME_MAP_H

#include <stdbool.h>
#include <stdint.h>

#define MAP_WIDTH  64
#define MAP_HEIGHT 64
#define TILE_SIZE  32
#define MAP_DEFAULT_SEED 1u
#define PLAYER_RADIUS 12.0f

typedef enum {
    TILE_GROUND = 0,
    TILE_WALL   = 1
} TileType;

typedef struct {
    uint8_t tiles[MAP_HEIGHT][MAP_WIDTH];
    uint32_t seed;
} GameMap;

// Init the map with a given seed
void map_init(GameMap *map, uint32_t seed);

// Return the tile type at the given coordinates
TileType map_get_tile(
    const GameMap *map,
    int x,
    int y
);

// Test if the tile at the given coordinates is solid 
bool map_is_solid(
    const GameMap *map,
    int x,
    int y
);

// Test if a circle collides with any solid tile in the map
bool map_collides_circle(
    const GameMap *map,
    float x,
    float y,
    float radius
);

// Move a circle with collision checks, sliding along walls.
void map_move_circle(
    const GameMap *map,
    float *x,
    float *y,
    float dx,
    float dy,
    float radius
);

#endif