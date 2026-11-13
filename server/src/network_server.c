#include <enet/enet.h>
#include <stdint.h>
#include <unistd.h>
#include "../include/network_server.h"
#include "../../shared/include/protocol.h"
#include <stdio.h>
#include <string.h>
#include "../include/world.h"
#include "player.h"
#include "world.h"

int network_server_init(NetworkServer *network)
{
    network->host = NULL;

    if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }

    ENetAddress address;
    ENetHost * server;

    address.host = ENET_HOST_ANY;
    address.port = GAME_PORT;

    network->host = enet_host_create (& address /* the address to bind the server host to */,
                                 MAX_PLAYERS      /* allow up to 32 clients and/or outgoing connections */,
                                  ENET_CHANNEL_COUNT      /* allow up to 2 channels to be used, 0 and 1 */,
                                  0      /* assume any amount of incoming bandwidth */,
                                  0      /* assume any amount of outgoing bandwidth */);
    if (network->host == NULL)
    {
        fprintf (stderr,
                 "An error occurred while trying to create an ENet server host.\n");
        exit (EXIT_FAILURE);
    }
return 0;
}

int network_server_poll(NetworkServer *network, World *world)
{
    ENetEvent event;

    int result = enet_host_service(
        network->host,
        &event,
        10
    );

    if (result < 0) {
        fprintf(stderr, "Erreur reseau ENet\n");
        return -1;
    }

    if (result == 0)
        return 0;

    switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
            printf("Nouveau client connecte\n");
            Player *player = world_add_player(world);
            printf(
                "Player [%u] | X: %.2f | Y: %.2f | Speed: %.2f | HP: %d | Active: %d\n",
                player->id,
                player->pos.x,
                player->pos.y,
                player->speed,
                player->health,
                player->active
            );

            network_broadcast_player_state( network, player);
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            printf("Paquet recu : %zu octets\n",
                   event.packet->dataLength);

            enet_packet_destroy(event.packet);
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            printf("Client deconnecte\n");
            break;

        default:
            break;
    }

    return 0;
}

void network_server_close(NetworkServer *network)
{
    if (network->host != NULL) {
        enet_host_destroy(network->host);
        network->host = NULL;
    }

    enet_deinitialize();

    printf("Serveur arrete\n");
}

void network_broadcast_player_state(
    NetworkServer *network,
    Player *player
)
{
    PlayerStatePacket state = {
        .type = PACKET_PLAYER_STATE,
        .id = player->id,
        .x = player->pos.x,
        .y = player->pos.y,
        .health = player->health,
        .active = player->active
    };

    ENetPacket *packet = enet_packet_create(
        &state,
        sizeof(PlayerStatePacket),
        0
    );

    if (packet != NULL)
        enet_host_broadcast(network->host, 1, packet);
}
