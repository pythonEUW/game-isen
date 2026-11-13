#include "game.h"
#include "map.h"
#include <math.h>
#include <raylib.h>

void game_run(void)
{
    GameMap map;
    map_init(&map, MAP_DEFAULT_SEED);
    Vector2 player = {100.0f, 100.0f};
    Camera2D camera = {0};
    camera.zoom = 1.0f;
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        //TODO: Delete this code and replace it with a proper game loop that handles player input, updates the game state, and renders the scene.
        float dx = (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) -
                   (IsKeyDown(KEY_A) || IsKeyDown(KEY_Q) || IsKeyDown(KEY_LEFT));
        float dy = (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) -
                   (IsKeyDown(KEY_W) || IsKeyDown(KEY_Z) || IsKeyDown(KEY_UP));
        float length = sqrtf(dx * dx + dy * dy);
        if (length > 1.0f) { dx /= length; dy /= length; }
        float dt = fminf(GetFrameTime(), 0.1f);
        map_move_circle(&map, &player.x, &player.y,
                        dx * 200.0f * dt, dy * 200.0f * dt, PLAYER_RADIUS);
        camera.offset = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
        camera.target.x = fmaxf(camera.offset.x,
            fminf(player.x, MAP_WIDTH * TILE_SIZE - camera.offset.x));
        camera.target.y = fmaxf(camera.offset.y,
            fminf(player.y, MAP_HEIGHT * TILE_SIZE - camera.offset.y));

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                Color color = map_is_solid(&map, x, y) ?
                    (Color){70, 78, 89, 255} :
                    ((x + y) % 2 ? (Color){40, 64, 44, 255} :
                                   (Color){44, 69, 48, 255});
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, color);
            }
        }
        DrawCircleV(player, PLAYER_RADIUS, SKYBLUE);
        DrawCircleLines((int)player.x, (int)player.y, PLAYER_RADIUS, WHITE);
        EndMode2D();
        
        EndDrawing();
    }
}
