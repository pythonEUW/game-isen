#include "menu.h"

#include <raylib.h>

#define SCREEN_W 800
#define SCREEN_H 450

#define NB_ITEMS 4

static const char *ITEM_LABELS[NB_ITEMS] = {
    "Jouer",
    "Options",
    "Credits",
    "Quitter",
};

static void placeholder_screen(const char *title, const char *line1, const char *line2)
{
    while (!WindowShouldClose() && !IsKeyPressed(KEY_ESCAPE))
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText(title, SCREEN_W / 2 - MeasureText(title, 40) / 2, 100, 40, DARKGRAY);
        DrawText(line1, SCREEN_W / 2 - MeasureText(line1, 20) / 2, 220, 20, GRAY);
        DrawText(line2, SCREEN_W / 2 - MeasureText(line2, 20) / 2, 250, 20, GRAY);
        DrawText("Echap pour revenir", SCREEN_W / 2 - MeasureText("Echap pour revenir", 16) / 2,
                 SCREEN_H - 50, 16, LIGHTGRAY);
        EndDrawing();
    }
}

MenuAction run_menu(void)
{
    int selected = 0;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
            selected = (selected + NB_ITEMS - 1) % NB_ITEMS;
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
            selected = (selected + 1) % NB_ITEMS;

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        {
            switch (selected)
            {
            case 0:
                return MENU_ACTION_PLAY;
            case 1:
                placeholder_screen("Options", "(a implementer)", "");
                break;
            case 2:
                placeholder_screen("Credits", "Game ISEN - Louison, Nassim, Luke, Lukas", "");
                break;
            case 3:
                return MENU_ACTION_QUIT;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        const char *title = "GAME ISEN";
        DrawText(title, SCREEN_W / 2 - MeasureText(title, 60) / 2, 60, 60, DARKGRAY);

        for (int i = 0; i < NB_ITEMS; i++)
        {
            int y = 180 + i * 50;
            Color c = (i == selected) ? RED : DARKGRAY;
            const char *label = ITEM_LABELS[i];
            int x = SCREEN_W / 2 - MeasureText(label, 30) / 2;
            if (i == selected)
                DrawText(">", x - 30, y, 30, c);
            DrawText(label, x, y, 30, c);
        }

        EndDrawing();
    }

    return MENU_ACTION_QUIT;
}
