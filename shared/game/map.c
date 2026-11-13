
#include "map.h"

#include <math.h>
#include <stddef.h>

void map_init(GameMap *map, uint32_t seed)
{
    if (map == NULL)
        return;

    map->seed = seed;

    // Fill the map with ground tiles and walls on the borders.
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {

            bool border =
                x == 0 ||
                y == 0 ||
                x == MAP_WIDTH - 1 ||
                y == MAP_HEIGHT - 1;

            map->tiles[y][x] =
                border ? TILE_WALL : TILE_GROUND;
        }
    }

    // Add some walls for testing purposes.
    for (int x = 10; x < 20; x++) {
        map->tiles[15][x] = TILE_WALL;
    }

    for (int y = 20; y < 30; y++) {
        map->tiles[y][25] = TILE_WALL;
    }

    // Add a square wall block in the middle of the map.
    for (int y = 35; y < 40; y++) {
        for (int x = 35; x < 40; x++) {
            map->tiles[y][x] = TILE_WALL;
        }
    }
}

TileType map_get_tile(
    const GameMap *map,
    int x,
    int y
)
{
    if (map == NULL ||
        x < 0 || x >= MAP_WIDTH ||
        y < 0 || y >= MAP_HEIGHT) {
        return TILE_WALL;
    }

    return (TileType)map->tiles[y][x];
}

bool map_is_solid(
    const GameMap *map,
    int x,
    int y
)
{
    return map_get_tile(map, x, y) == TILE_WALL;
}

bool map_collides_circle(
    const GameMap *map,
    float x,
    float y,
    float radius
)
{
    if (map == NULL ||
        !isfinite(x) || !isfinite(y) ||
        !isfinite(radius) || radius < 0.0f) {
        return true;
    }

    // Bound coordinates before converting to tile indices.
    if (x - radius < 0.0f || y - radius < 0.0f ||
        x + radius >= MAP_WIDTH * TILE_SIZE ||
        y + radius >= MAP_HEIGHT * TILE_SIZE)
        return true;

    int min_x =
        (int)floorf((x - radius) / TILE_SIZE);

    int max_x =
        (int)floorf((x + radius) / TILE_SIZE);

    int min_y =
        (int)floorf((y - radius) / TILE_SIZE);

    int max_y =
        (int)floorf((y + radius) / TILE_SIZE);

    for (int ty = min_y; ty <= max_y; ty++) {
        for (int tx = min_x; tx <= max_x; tx++) {

            if (!map_is_solid(map, tx, ty))
                continue;

            float left = tx * TILE_SIZE;
            float top = ty * TILE_SIZE;

            float closest_x = fmaxf(
                left,
                fminf(x, left + TILE_SIZE)
            );

            float closest_y = fmaxf(
                top,
                fminf(y, top + TILE_SIZE)
            );

            float dx = x - closest_x;
            float dy = y - closest_y;

            if (dx * dx + dy * dy < radius * radius)
                return true;
        }
    }

    return false;
}

void map_move_circle(const GameMap *map, float *x, float *y,
                     float dx, float dy, float radius)
{
    if (x == NULL || y == NULL || !isfinite(dx) || !isfinite(dy) ||
        map_collides_circle(map, *x, *y, radius))
        return;

    // Small steps prevent crossing a wall during a long frame.
    float distance = fmaxf(fabsf(dx), fabsf(dy));
    if (distance > MAP_WIDTH * TILE_SIZE + MAP_HEIGHT * TILE_SIZE)
        return;
    int steps = (int)ceilf(distance / (TILE_SIZE / 4.0f));
    if (steps < 1) steps = 1;
    dx /= steps;
    dy /= steps;
    for (int i = 0; i < steps; i++) {
        if (!map_collides_circle(map, *x + dx, *y, radius))
            *x += dx;
        if (!map_collides_circle(map, *x, *y + dy, radius))
            *y += dy;
    }
}
