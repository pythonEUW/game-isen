#include "game.h"

#include <raylib.h>

#define SCREEN_W 800
#define SCREEN_H 450

void run_game(NetworkClient *nc)
{
    (void)nc; /* le reseau sera utilise ici plus tard */
    network_client_send(nc, "Client a lancer le jeu");

    while (!WindowShouldClose() && !IsKeyPressed(KEY_ESCAPE))
    {
        network_client_poll(nc); /* entretient la connexion et lit les reponses */

        BeginDrawing();
        ClearBackground((Color){30, 30, 40, 255});
        DrawText("LE JEU", SCREEN_W / 2 - MeasureText("LE JEU", 40) / 2, 180, 40, RAYWHITE);
        DrawText("Echap pour revenir au menu",
                 SCREEN_W / 2 - MeasureText("Echap pour revenir au menu", 16) / 2,
                 SCREEN_H - 50, 16, LIGHTGRAY);
        EndDrawing();
    }
}
