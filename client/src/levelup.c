#include "levelup.h"

#include <raylib.h>

#define CARD_W   200.0f
#define CARD_H   260.0f
#define CARD_GAP 30.0f

static Rectangle card_rect(int index)
{
    float total = UPGRADE_CHOICES * CARD_W + (UPGRADE_CHOICES - 1) * CARD_GAP;
    float x = (GetScreenWidth() - total) / 2.0f + index * (CARD_W + CARD_GAP);
    float y = (GetScreenHeight() - CARD_H) / 2.0f + 20.0f;
    return (Rectangle){x, y, CARD_W, CARD_H};
}

int levelup_update(LevelUpScreen *screen)
{
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_Q))
        screen->selected = (screen->selected + UPGRADE_CHOICES - 1) % UPGRADE_CHOICES;
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
        screen->selected = (screen->selected + 1) % UPGRADE_CHOICES;

    /* Raccourcis directs 1 / 2 / 3 (rangee du haut ou pave numerique). */
    for (int i = 0; i < UPGRADE_CHOICES; i++)
        if (IsKeyPressed(KEY_ONE + i) || IsKeyPressed(KEY_KP_1 + i))
            return i;

    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < UPGRADE_CHOICES; i++) {
        if (CheckCollisionPointRec(mouse, card_rect(i))) {
            screen->selected = i;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                return i;
        }
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        return screen->selected;
    return -1;
}

void levelup_draw(const LevelUpScreen *screen, uint8_t level,
                  const uint8_t choices[UPGRADE_CHOICES])
{
    int sw = GetScreenWidth();
    DrawRectangle(0, 0, sw, GetScreenHeight(), (Color){0, 0, 0, 170});

    const char *title = level > 0 ? TextFormat("NIVEAU %u !", level) : "NIVEAU SUPERIEUR !";
    DrawText(title, sw / 2 - MeasureText(title, 36) / 2, 40, 36, GOLD);
    const char *hint = "1/2/3, fleches + Entree, ou clic";
    DrawText(hint, sw / 2 - MeasureText(hint, 16) / 2, 82, 16, LIGHTGRAY);

    for (int i = 0; i < UPGRADE_CHOICES; i++) {
        Rectangle r = card_rect(i);
        bool sel = i == screen->selected;
        if (sel) r.y -= 8.0f; /* la carte surlignee se souleve un peu */

        DrawRectangleRounded(r, 0.08f, 8, sel ? (Color){48, 54, 70, 255} : (Color){30, 34, 44, 255});
        DrawRectangleRoundedLinesEx(r, 0.08f, 8, sel ? 4.0f : 2.0f, sel ? GOLD : GRAY);

        int cx = (int)(r.x + r.width / 2);
        const char *key = TextFormat("%d", i + 1);
        DrawText(key, cx - MeasureText(key, 20) / 2, (int)r.y + 14, 20, sel ? GOLD : GRAY);

        const char *name = upgrade_name((UpgradeId)choices[i]);
        DrawText(name, cx - MeasureText(name, 22) / 2, (int)r.y + 110, 22, WHITE);

        /* Description centree ligne par ligne (separees par '\n'). */
        const char *desc = upgrade_description((UpgradeId)choices[i]);
        int line_count = 0;
        char **lines = TextSplit(desc, '\n', &line_count);
        for (int l = 0; l < line_count; l++)
            DrawText(lines[l], cx - MeasureText(lines[l], 16) / 2,
                     (int)r.y + 150 + l * 20, 16, LIGHTGRAY);
    }
}
