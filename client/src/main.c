#include <stdio.h>
#include <stdlib.h>
#include <enet/enet.h>
#include <raylib.h>

#include "protocol.h"
#include "network_client.h"
#include "menu.h"
#include "game.h"

int main(void)
{
    if (enet_initialize() != 0) {
        fprintf(stderr, "ENet: erreur d'initialisation.\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);
    NetworkClient nc = {0};
    if (network_client_connect(&nc, "127.0.0.1", GAME_PORT) != 0)
        fprintf(stderr, "Serveur absent : exploration locale disponible.\n");

    InitWindow(800, 450, "Game ISEN");
    if (!IsWindowReady()) {
        network_client_disconnect(&nc);
        return EXIT_FAILURE;
    }
    // Escape belongs to the menu/game navigation; the close button still quits.
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        if (run_menu(&nc) == MENU_ACTION_QUIT)
            break;
        if (!WindowShouldClose())
            game_run(&nc);
    }
    network_client_disconnect(&nc);
    CloseWindow();
    return EXIT_SUCCESS;
}
