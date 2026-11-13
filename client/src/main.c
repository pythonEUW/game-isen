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

#include "game.h"

/* Test ENet : connexion a un serveur sur 127.0.0.1:GAME_PORT,
   envoi d'un paquet, puis deconnexion propre.
   A lancer avec ./build/server dans un autre terminal. */
static void test_enet(void)
{
    ENetHost *client = enet_host_create(NULL, 1, ENET_CHANNEL_COUNT, 0, 0);
    if (client == NULL)
    {
        fprintf(stderr, "ENet: impossible de creer le host client.\n");
        return;
    }

    ENetAddress address;
    enet_address_set_host(&address, "127.0.0.1");
    address.port = GAME_PORT;

    ENetPeer *peer = enet_host_connect(client, &address, ENET_CHANNEL_COUNT, 0);
    if (peer == NULL)
    {
        fprintf(stderr, "ENet: enet_host_connect a echoue.\n");
        enet_host_destroy(client);
        return;
    }

    ENetEvent event;
    if (enet_host_service(client, &event, 3000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        printf("ENet OK: connecte au serveur 127.0.0.1:%d\n", GAME_PORT);

        ENetPacket *packet = enet_packet_create("ping", 5, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
        enet_host_flush(client);
        printf("ENet OK: paquet 'ping' envoye\n");

        enet_peer_disconnect(peer, 0);
        /* attend l'evenement de deconnexion (ou timeout) */
        while (enet_host_service(client, &event, 1000) > 0 &&
               event.type != ENET_EVENT_TYPE_DISCONNECT)
        {
        }
        printf("ENet OK: deconnecte proprement\n");
    }
    else
    {
        printf("ENet: serveur non atteignable sur 127.0.0.1:%d "
               "(lance ./build/server dans un autre terminal)\n", GAME_PORT);
    }

    enet_host_destroy(client);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "ENet: erreur d'initialisation.\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    test_enet();

    NetworkClient nc = {0};
    // lance la connexion si impossible on ferme la fenetre 
    if (network_client_connect(&nc, "127.0.0.1", GAME_PORT) != 0){
        fprintf(stderr, "Client: connexion impossible .\n");
        CloseWindow();
        return EXIT_FAILURE;
    }

    InitWindow(800, 450, "Game ISEN");
    if (!IsWindowReady())
    {
        fprintf(stderr, "raylib: impossible d'ouvrir la fenetre.\n");
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
            game_run(&nc);
    }

    network_client_disconnect(&nc);

    CloseWindow();
    return EXIT_SUCCESS;
}
