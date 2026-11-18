#include "levelup.h"

#include <string.h>

#ifndef ASSETS_DIR
#define ASSETS_DIR "assets"
#endif
#define CARDS_DIR ASSETS_DIR "/cards/"

/* Geometrie des sprites (en pixels de la spritesheet d'origine).
 * Cadre 68x95 : fenetre d'illustration en arche 64x49 en (2,2),
 * fenetre de texte 62x36 en (3,57). */
#define FRAME_W 68
#define FRAME_H 95
#define ART_X 2
#define ART_Y 2
#define ART_W 64
#define ART_H 49
#define TEXT_X 3
#define TEXT_Y 57
#define TEXT_W 62

/* Taille a l'ecran : 1.5 => cartes de 102x142 px, assez petites pour jouer. */
#define CARD_SCALE  1.5f
#define CARD_GAP    10.0f
#define CARD_MARGIN 8.0f /* marge entre les cartes et le bas de l'ecran */

#define CARD_W (FRAME_W * CARD_SCALE)
#define CARD_H (FRAME_H * CARD_SCALE)

/* Niveau 1 -> 8 : du plus commun au plus rare. */
static const char *LEVEL_NAMES[CARD_LEVELS] = {
    "stone", "green", "lightblue", "blue", "purple", "pink", "red", "gold",
};
/* Fond de l'illustration, une teinte sombre de la couleur du cadre. */
static const Color LEVEL_BG[CARD_LEVELS] = {
    {74, 66, 58, 255},  {22, 66, 20, 255},  {44, 84, 122, 255}, {16, 48, 96, 255},
    {44, 22, 68, 255},  {96, 30, 104, 255}, {86, 14, 14, 255},  {104, 80, 12, 255},
};

/* Illustration de chaque amelioration : sprite, zoom (en multiples de la
 * taille d'origine) et rotation en degres. */
static const struct {
    const char *file;
    float zoom;
    float rotation;
} UPGRADE_ART[UPGRADE_COUNT] = {
    [UPGRADE_NEW_WEAPON]   = {"icons/weapon_sword.png",          0.85f, -35.0f},
    [UPGRADE_MAX_HEALTH]   = {"icons/gem_red.png",               5.0f,  0.0f},
    [UPGRADE_SPEED]        = {"icons/weapon_arrow_fletched.png", 0.85f, -20.0f},
    [UPGRADE_DAMAGE]       = {"icons/round_sword.png",           2.5f,  0.0f},
    [UPGRADE_ATTACK_SPEED] = {"icons/round_arrow.png",           2.5f,  0.0f},
};

static Texture2D load(const char *relative, bool *ok)
{
    Texture2D tex = LoadTexture(TextFormat("%s%s", CARDS_DIR, relative));
    if (tex.id == 0) *ok = false;
    return tex;
}

void levelup_load(LevelUpScreen *screen)
{
    bool ok = true;
    for (int i = 0; i < CARD_LEVELS; i++) {
        const char *n = LEVEL_NAMES[i];
        screen->frames[i] = load(TextFormat("frames/level%d_%s.png", i + 1, n), &ok);
        screen->frames_ornate[i] = load(TextFormat("frames/level%d_%s_ornate.png", i + 1, n), &ok);
        screen->badges[i] = load(TextFormat("badges/level%d_%s.png", i + 1, n), &ok);
    }
    for (int i = 0; i < 9; i++)
        screen->digits[i] = load(TextFormat("digits/%d.png", i + 1), &ok);
    screen->art_border = load("parts/art_border_gold.png", &ok);
    screen->text_plate = load("parts/text_plate_gold.png", &ok);
    for (int i = 0; i < UPGRADE_COUNT; i++)
        screen->icons[i] = load(UPGRADE_ART[i].file, &ok);

    screen->loaded = ok;
    if (!ok)
        TraceLog(LOG_WARNING, "levelup: sprites introuvables dans %s, dessin simplifie", CARDS_DIR);
}

void levelup_unload(LevelUpScreen *screen)
{
    /* UnloadTexture ignore les textures d'id 0 (echec de chargement). */
    for (int i = 0; i < CARD_LEVELS; i++) {
        UnloadTexture(screen->frames[i]);
        UnloadTexture(screen->frames_ornate[i]);
        UnloadTexture(screen->badges[i]);
    }
    for (int i = 0; i < 9; i++)
        UnloadTexture(screen->digits[i]);
    UnloadTexture(screen->art_border);
    UnloadTexture(screen->text_plate);
    for (int i = 0; i < UPGRADE_COUNT; i++)
        UnloadTexture(screen->icons[i]);
    screen->loaded = false;
}

static Rectangle card_rect(int index)
{
    float total = UPGRADE_CHOICES * CARD_W + (UPGRADE_CHOICES - 1) * CARD_GAP;
    float x = (GetScreenWidth() - total) / 2.0f + index * (CARD_W + CARD_GAP);
    float y = GetScreenHeight() - CARD_H - CARD_MARGIN;
    return (Rectangle){x, y, CARD_W, CARD_H};
}

int levelup_update(LevelUpScreen *screen, const uint8_t choices[UPGRADE_CHOICES])
{
    int picked = -1;

    /* Raccourcis directs 1 / 2 / 3 (rangee du haut ou pave numerique). */
    for (int i = 0; i < UPGRADE_CHOICES && picked < 0; i++)
        if (IsKeyPressed(KEY_ONE + i) || IsKeyPressed(KEY_KP_1 + i))
            picked = i;

    /* Survol souris = surlignage, clic = validation. */
    screen->selected = -1;
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < UPGRADE_CHOICES; i++) {
        if (CheckCollisionPointRec(mouse, card_rect(i))) {
            screen->selected = i;
            if (picked < 0 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                picked = i;
        }
    }

    if (picked >= 0 && choices[picked] < UPGRADE_COUNT && screen->owned[choices[picked]] < 255)
        screen->owned[choices[picked]]++;
    return picked;
}

/* Dessine un sprite a l'echelle de la carte, position en pixels de sprite. */
static void blit(Texture2D tex, Rectangle card, float sx, float sy)
{
    DrawTextureEx(tex, (Vector2){card.x + sx * CARD_SCALE, card.y + sy * CARD_SCALE},
                  0.0f, CARD_SCALE, WHITE);
}

static void draw_centered(const char *text, int cx, int y, int size, Color color)
{
    DrawText(text, cx - MeasureText(text, size) / 2, y, size, color);
}

static void draw_card(const LevelUpScreen *screen, Rectangle r, int index,
                      UpgradeId id, bool sel)
{
    int level = screen->owned[id] + 1;                 /* niveau que donnera la carte */
    int tier = (level - 1) % CARD_LEVELS;              /* couleur du cadre */
    bool ornate = level > CARD_LEVELS;                 /* au-dela de 8 : cadre orne */
    int cx = (int)(r.x + r.width / 2);

    /* 1. Fond de l'illustration (les coins hors de l'arche sont caches par le cadre). */
    DrawRectangleRec((Rectangle){r.x + ART_X * CARD_SCALE, r.y + ART_Y * CARD_SCALE,
                                 ART_W * CARD_SCALE, ART_H * CARD_SCALE}, LEVEL_BG[tier]);

    /* 2. Illustration de l'amelioration, centree dans l'arche. */
    Texture2D icon = screen->icons[id];
    float zoom = UPGRADE_ART[id].zoom * CARD_SCALE;
    Rectangle src = {0, 0, (float)icon.width, (float)icon.height};
    Rectangle dst = {r.x + (ART_X + ART_W / 2.0f) * CARD_SCALE,
                     r.y + (ART_Y + ART_H / 2.0f + 3.0f) * CARD_SCALE,
                     icon.width * zoom, icon.height * zoom};
    DrawTexturePro(icon, src, dst, (Vector2){dst.width / 2, dst.height / 2},
                   UPGRADE_ART[id].rotation, WHITE);

    /* 3. Plaque de texte, cadre de rarete et lisere de l'arche. */
    blit(screen->text_plate, r, TEXT_X, TEXT_Y);
    blit(ornate ? screen->frames_ornate[tier] : screen->frames[tier], r, 0, 0);
    blit(screen->art_border, r, ART_X, ART_Y);

    /* 4. Chevron de niveau en haut a gauche, avec le chiffre dessus. */
    blit(screen->badges[tier], r, 4, 0);
    if (level <= 9)
        blit(screen->digits[level - 1], r, 8, 2);
    else
        DrawText(TextFormat("%d", level), (int)(r.x + 6 * CARD_SCALE), (int)(r.y + 3 * CARD_SCALE), 10, WHITE);

    /* 5. Touche a utiliser, en haut a droite. */
    Vector2 key = {r.x + (FRAME_W - 9) * CARD_SCALE, r.y + 9 * CARD_SCALE};
    DrawCircleV(key, 7.0f, sel ? GOLD : (Color){20, 20, 20, 220});
    draw_centered(TextFormat("%d", index + 1), (int)key.x + 1, (int)key.y - 4, 10,
                  sel ? BLACK : WHITE);

    /* 6. Nom et description sur la plaque. */
    int text_y = (int)(r.y + TEXT_Y * CARD_SCALE);
    const Color ink = {58, 36, 16, 255};
    draw_centered(upgrade_name(id), cx, text_y + 6, 10, ink);
    int line_count = 0;
    char **lines = TextSplit(upgrade_description(id), '\n', &line_count);
    for (int l = 0; l < line_count; l++)
        draw_centered(lines[l], cx, text_y + 22 + l * 11, 10, (Color){96, 72, 48, 255});
}

/* Dessin de secours si les sprites n'ont pas pu etre charges. */
static void draw_card_fallback(Rectangle r, int index, UpgradeId id, bool sel)
{
    int cx = (int)(r.x + r.width / 2);
    DrawRectangleRounded(r, 0.1f, 6, sel ? (Color){48, 54, 70, 230} : (Color){30, 34, 44, 200});
    DrawRectangleRoundedLinesEx(r, 0.1f, 6, sel ? 3.0f : 1.5f, sel ? GOLD : GRAY);
    DrawText(TextFormat("%d", index + 1), (int)r.x + 8, (int)r.y + 6, 16, sel ? GOLD : GRAY);
    draw_centered(upgrade_name(id), cx, (int)r.y + 40, 12, WHITE);
    int line_count = 0;
    char **lines = TextSplit(upgrade_description(id), '\n', &line_count);
    for (int l = 0; l < line_count; l++)
        draw_centered(lines[l], cx, (int)r.y + 62 + l * 13, 10, LIGHTGRAY);
}

void levelup_draw(const LevelUpScreen *screen, uint8_t level,
                  const uint8_t choices[UPGRADE_CHOICES])
{
    /* Petit titre au-dessus des cartes, pas de voile plein ecran. */
    const char *title = level > 0 ? TextFormat("Niveau %u ! Choisis : 1 / 2 / 3", level)
                                  : "Niveau superieur ! Choisis : 1 / 2 / 3";
    int title_y = (int)card_rect(0).y - 26;
    int sw = GetScreenWidth();
    draw_centered(title, sw / 2 + 1, title_y + 1, 16, BLACK);
    draw_centered(title, sw / 2, title_y, 16, GOLD);

    for (int i = 0; i < UPGRADE_CHOICES; i++) {
        UpgradeId id = (UpgradeId)choices[i];
        if (id >= UPGRADE_COUNT) continue;
        Rectangle r = card_rect(i);
        bool sel = i == screen->selected;
        if (sel) r.y -= 8.0f; /* la carte survolee se souleve */

        /* Ombre portee pour detacher la carte du decor. */
        DrawRectangleRec((Rectangle){r.x + 4, r.y + 4, r.width, r.height}, (Color){0, 0, 0, 90});

        if (screen->loaded)
            draw_card(screen, r, i, id, sel);
        else
            draw_card_fallback(r, i, id, sel);
    }
}
