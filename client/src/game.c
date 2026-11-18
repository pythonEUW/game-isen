#include "game.h"
#include "levelup.h"
#include "map.h"
#include "protocol.h"
#include <math.h>
#include <stdio.h>
#include <raylib.h>

void game_run(NetworkClient *nc)
{
    GameMap map;
    map_init(&map, MAP_DEFAULT_SEED);
    Vector2 player = {100.0f, 100.0f};
    Camera2D camera = {0};
    camera.zoom = 1.0f;
    SetTargetFPS(60);

    LevelUpScreen levelup = {0};
    levelup_load(&levelup);
    bool debug_offer = false;
    uint8_t debug_choices[UPGRADE_CHOICES];

    while (!WindowShouldClose() && !IsKeyPressed(KEY_ESCAPE)) {
        network_client_poll(nc); /* entretient la connexion et lit les reponses */

#ifndef NDEBUG
        if (IsKeyPressed(KEY_F1) && !nc->levelup_pending && !debug_offer) {
            upgrade_roll_choices(debug_choices);
            debug_offer = true;
        }
#endif

        /* Choix de carte non bloquant : on continue a lire le deplacement. */
        if (nc->levelup_pending || debug_offer) {
            int picked = levelup_update(&levelup, debug_offer ? debug_choices
                                                              : nc->levelup_choices);
            if (picked >= 0) {
                if (debug_offer) {
                    printf("[debug] carte choisie : %s\n",
                           upgrade_name((UpgradeId)debug_choices[picked]));
                    debug_offer = false;
                } else {
                    network_client_send_upgrade_choice(nc, (uint8_t)picked);
                }
            }
        }
        float dx = (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) -
                   (IsKeyDown(KEY_A) || IsKeyDown(KEY_Q) || IsKeyDown(KEY_LEFT));
        float dy = (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) -
                   (IsKeyDown(KEY_W) || IsKeyDown(KEY_Z) || IsKeyDown(KEY_UP));
        network_client_send_input(nc, (int8_t)dx, (int8_t)dy);
        if (nc->peer != NULL) {
            for (int i = 0; i < MAX_PLAYERS; i++)
                if (nc->players[i].active && nc->players[i].id == nc->player_id)
                    player = (Vector2){nc->players[i].x, nc->players[i].y};
        } else {
            float length = sqrtf(dx * dx + dy * dy);
            if (length > 1.0f) { dx /= length; dy /= length; }
            float dt = fminf(GetFrameTime(), 0.1f);
            map_move_circle(&map, &player.x, &player.y,
                            dx * 200.0f * dt, dy * 200.0f * dt, PLAYER_RADIUS);
        }
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
        int count = 0;
        if (nc->peer != NULL) {
            const Color colors[MAX_PLAYERS] = {SKYBLUE, ORANGE, LIME, VIOLET};
            for (int i = 0; i < MAX_PLAYERS; i++) {
                const PlayerStatePacket *other = &nc->players[i];
                if (!other->active) continue;
                count++;
                bool self = other->id == nc->player_id;
                Vector2 pos = {other->x, other->y};
                DrawCircleV(pos, PLAYER_RADIUS, colors[(other->id - 1) % MAX_PLAYERS]);
                DrawCircleLines((int)pos.x, (int)pos.y, PLAYER_RADIUS, self ? WHITE : DARKGRAY);
                DrawText(TextFormat("J%u%s", other->id, self ? " (toi)" : ""),
                         (int)pos.x - 18, (int)pos.y - 30, 16, WHITE);
            }
        } else {
            DrawCircleV(player, PLAYER_RADIUS, SKYBLUE);
        }
        EndMode2D();
                
        DrawText(nc->peer != NULL ? TextFormat("En ligne : %d/%d joueurs", count, MAX_PLAYERS) :
                 "Hors ligne - exploration locale", 12, 34, 16, LIGHTGRAY);

        if (debug_offer)
            levelup_draw(&levelup, 0, debug_choices);
        else if (nc->levelup_pending)
            levelup_draw(&levelup, nc->level, nc->levelup_choices);
        EndDrawing();
    }
    levelup_unload(&levelup);
    network_client_send_input(nc, 0, 0);
}
