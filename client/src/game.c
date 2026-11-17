#include "game.h"
#include "game_config.h"
#include "map.h"
#include "protocol.h"
#include <math.h>
#include <raylib.h>

#define SPRITE_NATIVE 192 /* taille native d'une frame dans les PNG Tiny Swords */
#define SPRITE_DRAW   72  /* taille d'affichage du sprite (48 x 1.5) */
#define RUN_FPS       8   /* images par seconde de la course */
#define IDLE_FPS      4   /* images par seconde de l'inaction */

/* Une spritesheet d'animation : frames carrees de SPRITE_NATIVE px sur une
   seule ligne horizontale (direction droite ; on retourne pour la gauche). */
typedef struct {
    Texture2D sheet;
    int frames;
    float fps;
} AnimSheet;

static void anim_load(AnimSheet *a, const char *path, float fps)
{
    a->sheet = LoadTexture(path);
    if (!IsTextureValid(a->sheet))
        a->sheet = LoadTexture(TextFormat("../%s", path)); /* lance depuis build/ */
    a->frames = IsTextureValid(a->sheet) ? a->sheet.width / SPRITE_NATIVE : 0;
    a->fps = fps;
}

static void anim_unload(AnimSheet *a)
{
    if (IsTextureValid(a->sheet))
        UnloadTexture(a->sheet);
}

/* Dessine une frame centree sur le cercle de collision (pos = son centre).
   `facing` = -1 retourne le sprite horizontalement.
   Retombe sur un cercle si la texture est absente. */
static void draw_anim(const AnimSheet *a, int frame, float facing, Vector2 pos, Color tint)
{
    if (IsTextureValid(a->sheet) && a->frames > 0) {
        Rectangle src = {(float)(frame % a->frames) * SPRITE_NATIVE, 0.0f,
                         facing * SPRITE_NATIVE, (float)SPRITE_NATIVE};
        Rectangle dst = {pos.x - SPRITE_DRAW / 2.0f,
                         pos.y - SPRITE_DRAW / 2.0f,
                         SPRITE_DRAW, SPRITE_DRAW};
        DrawTexturePro(a->sheet, src, dst, (Vector2){0.0f, 0.0f}, 0.0f, tint);
    } else {
        DrawCircleV(pos, PLAYER_RADIUS, tint);
    }
}

void game_run(NetworkClient *nc)
{
    GameMap map;
    map_init(&map, MAP_DEFAULT_SEED);
    Vector2 player = {PLAYER_SPAWN_X, PLAYER_SPAWN_Y};
    Camera2D camera = {0};
    camera.zoom = 2.0f; /* > 1.0 = zoom sur le joueur (molette pour ajuster) */
    SetTargetFPS(GAME_TICK_RATE);

    /* Sprites Tiny Swords (Archer noir) : idle = 6 frames, run = 4 frames. */
    AnimSheet idle, run;
    anim_load(&idle, "assets/archer_idle.png", IDLE_FPS);
    anim_load(&run, "assets/archer_run.png", RUN_FPS);
    int frame = 0;
    float frame_timer = 0.0f;
    float facing = 1.0f;

    while (!WindowShouldClose() && !IsKeyPressed(KEY_ESCAPE)) {
        network_client_poll(nc); /* entretient la connexion et lit les reponses */


        float dx = (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) -
                   (IsKeyDown(KEY_A) || IsKeyDown(KEY_Q) || IsKeyDown(KEY_LEFT));
        float dy = (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) -
                   (IsKeyDown(KEY_W) || IsKeyDown(KEY_Z) || IsKeyDown(KEY_UP));
        network_client_send_input(nc, (int8_t)dx, (int8_t)dy);

        /* Animation : sheet "run" quand le joueur bouge, "idle" sinon */
        const AnimSheet *current = (dx != 0.0f || dy != 0.0f) ? &run : &idle;
        if (dx < 0.0f) facing = -1.0f;
        else if (dx > 0.0f) facing = 1.0f;
        if (current->frames > 0) {
            frame_timer += GetFrameTime();
            if (frame_timer >= 1.0f / current->fps) {
                frame_timer -= 1.0f / current->fps;
                frame = (frame + 1) % current->frames;
            }
        }

        if (nc->peer != NULL) {
            for (int i = 0; i < MAX_PLAYERS; i++)
                if (nc->players[i].active && nc->players[i].id == nc->player_id)
                    player = (Vector2){nc->players[i].x, nc->players[i].y};
        } else {
            float length = sqrtf(dx * dx + dy * dy);
            if (length > 1.0f) { dx /= length; dy /= length; }
            float dt = fminf(GetFrameTime(), 0.1f);
            map_move_circle(&map, &player.x, &player.y,
                            dx * PLAYER_SPEED * dt, dy * PLAYER_SPEED * dt, PLAYER_RADIUS);
        }
        camera.offset = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
        camera.zoom = fmaxf(1.0f, fminf(4.0f, camera.zoom + GetMouseWheelMove() * 0.25f));
        /* la zone visible a l'ecran fait offset/zoom en monde : on bloque la
           camera sur les bords de la map en tenant compte du zoom */
        float half_w = camera.offset.x / camera.zoom;
        float half_h = camera.offset.y / camera.zoom;
        camera.target.x = fmaxf(half_w,
            fminf(player.x, MAP_WIDTH * TILE_SIZE - half_w));
        camera.target.y = fmaxf(half_h,
            fminf(player.y, MAP_HEIGHT * TILE_SIZE - half_h));

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
            for (int i = 0; i < MAX_PLAYERS; i++) {
                const PlayerStatePacket *other = &nc->players[i];
                if (!other->active) continue;
                count++;
                bool self = other->id == nc->player_id;
                Vector2 pos = {other->x, other->y};
                draw_anim(current, frame, facing, pos, WHITE);
                DrawText(TextFormat("J%u%s", other->id, self ? " (toi)" : ""),
                         (int)pos.x - 18, (int)pos.y - 30, 10, WHITE);
            }
        } else {
            draw_anim(current, frame, facing, player, WHITE);
            
        }
        EndMode2D();
                
        DrawText(nc->peer != NULL ? TextFormat("En ligne : %d/%d joueurs", count, MAX_PLAYERS) :
                 "Hors ligne - exploration locale", 12, 34, 16, LIGHTGRAY);
        EndDrawing();
    }
    anim_unload(&idle);
    anim_unload(&run);
    network_client_send_input(nc, 0, 0);
}
