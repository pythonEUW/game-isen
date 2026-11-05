#include <stdio.h>
#include <stdlib.h>
#include <enet/enet.h>
#include <raylib.h>

#include "protocol.h"
#include "network_client.h"
#include "menu.h"
#include "game.h"

#define SCREEN_W 800
#define SCREEN_H 450

int main(void)
{
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "ENet: erreur d'initialisation.\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    NetworkClient nc = {0};
    // lance la connexion si impossible on ferme la fenetre 
    if (network_client_connect(&nc, "127.0.0.1", GAME_PORT) != 0){
        fprintf(stderr, "Client: connexion impossible .\n");
        CloseWindow();
        return EXIT_FAILURE;
    }
    InitWindow(SCREEN_W, SCREEN_H, "Game ISEN");
    SetTargetFPS(60);
    //lance menu et jeu si bouton jouer appuyé
    MenuAction action = MENU_ACTION_PLAY;
    while (action == MENU_ACTION_PLAY && !WindowShouldClose())
    {
        action = run_menu();
        if (action == MENU_ACTION_PLAY)
            run_game(&nc);
    }

    network_client_disconnect(&nc);
    CloseWindow();
    return EXIT_SUCCESS;
}
