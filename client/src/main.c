#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <enet/enet.h>
#include <raylib.h>

#include "protocol.h"

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

        /* Reste connecte quelques secondes pour recevoir les etats
           des joueurs broadcastes par le serveur (arrivees/departs). */
        for (int i = 0; i < 10; i++) {
            if (enet_host_service(client, &event, 1000) <= 0)
                continue;

            if (event.type == ENET_EVENT_TYPE_RECEIVE) {
                if (event.packet->dataLength >= sizeof(PlayerStatePacket)) {
                    PlayerStatePacket *state =
                        (PlayerStatePacket *)event.packet->data;
                    if (state->type == PACKET_PLAYER_STATE) {
                        printf("Player [%u] | X: %.2f | Y: %.2f | HP: %d | Active: %d\n",
                               state->id, state->x, state->y,
                               state->health, state->active);
                    }
                }
                enet_packet_destroy(event.packet);
            } else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
                break;
            }
        }

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
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    test_enet();

    InitWindow(800, 450, "Game ISEN - test raylib");
    if (!IsWindowReady())
    {
        fprintf(stderr, "raylib: impossible d'ouvrir la fenetre.\n");
        return EXIT_FAILURE;
    }
    SetTargetFPS(60);

    int frames = 0;
    /* fermeture auto apres 300 frames (~5 s) pour tester sans interaction */
    while (!WindowShouldClose() && frames < 300)
    {
        frames++;
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("raylib fonctionne !", 280, 220, 20, DARKGRAY);
        DrawRectangle(frames % 700 + 50, 100, 50, 50, RED);
        EndDrawing();
    }

    CloseWindow();
    printf("raylib OK: fenetre ouverte puis fermee proprement (%d frames)\n", frames);
    return EXIT_SUCCESS;
}
