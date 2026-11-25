#include <enet/enet.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/network_server.h"
#include "../../shared/include/protocol.h"
#include <stdio.h>
#include <string.h>
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
        case ENET_EVENT_TYPE_CONNECT: {
            printf("Nouveau client connecte\n");
            Player *player = world_add_player(world);
            if (player == NULL) {
                printf("Serveur plein, connexion ignoree\n");
                enet_peer_disconnect(event.peer, 0);
                break;
            }
            event.peer->data = (void *)(uintptr_t)player->id;
            WelcomePacket welcome = {.type = PACKET_WELCOME, .id = player->id};
            ENetPacket *greeting = enet_packet_create(&welcome, sizeof(welcome), ENET_PACKET_FLAG_RELIABLE);
            if (greeting != NULL && enet_peer_send(event.peer, 1, greeting) < 0)
                enet_packet_destroy(greeting);

            printf(
                "Player [%u] | X: %.2f | Y: %.2f | Speed: %.2f | HP: %d | LEVEL: %d | Active: %d\n",
                player->id,
                player->pos.x,
                player->pos.y,
                player->speed,
                player->health,
                player->level,
                player->active
            );

            // Synchroniser le nouveau client avec les joueurs deja presents,
            // puis signaler a tout le monde l'arrivee du nouveau joueur.
            network_broadcast_world_state(network, world);
            network_broadcast_player_state(network, player);
            break;
        }

        case ENET_EVENT_TYPE_RECEIVE: {
            if (event.packet->dataLength == sizeof(PlayerInputPacket)) {
                PlayerInputPacket *input =
                    (PlayerInputPacket *)event.packet->data;

                if (input->type == PACKET_PLAYER_INPUT) {
                    uint32_t id = (uint32_t)(uintptr_t)event.peer->data;
                    Player *player = world_find_player(world, id);

                    if (player != NULL) {
                        player->input_x = input->input_x;
                        player->input_y = input->input_y;

                        // Gain d'XP uniquement si le joueur se deplace.
                        if (player->input_x != 0 || player->input_y != 0) {
                            player->xp++;
                            while ((long long)player->xp >=
                                   xp_needed_for_level((int)player->level + 1)) {
                                player->level++;
                                printf("Joueur %u passe niveau %u\n",
                                       player->id, player->level);
                                // Paquet envoye uniquement a ce joueur (pas de broadcast).
                                network_send_level_up(network, player);
                            }
                        }
                    }
                }
            }

            enet_packet_destroy(event.packet);
            break;
        }

        case ENET_EVENT_TYPE_DISCONNECT: {
            printf("Client deconnecte\n");
            uint32_t id = (uint32_t)(uintptr_t)event.peer->data;
            Player *player = world_find_player(world, id);

            if (player != NULL) {
                world_remove_player(world, id);
                network_broadcast_player_state(network, player);
            }
            event.peer->data = NULL;
            break;
        }

        default:
            break;
    }

    // Force l'envoi immediat des paquets broadcastes pendant cet evenement,
    // sans attendre le prochain enet_host_service.
    enet_host_flush(network->host);

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
        .level = player->level,
        .xp = player->xp,
        .active = player->active
    };

    ENetPacket *packet = enet_packet_create(
        &state,
        sizeof(PlayerStatePacket),
        ENET_PACKET_FLAG_RELIABLE
    );

    if (packet != NULL)
        enet_host_broadcast(network->host, 1, packet);
}

void network_broadcast_world_state(
    NetworkServer *network,
    World *world
)
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player *player = &world->players[i];

        if (player->active)
            network_broadcast_player_state(network, player);
    }
}

void network_send_level_up(
    NetworkServer *network,
    Player *player
)
{
    // Retrouver le peer ENet du joueur via peer->data (stocké à la connexion).
    ENetPeer *peer = NULL;
    for (size_t i = 0; i < network->host->peerCount; i++) {
        ENetPeer *p = &network->host->peers[i];
        if (p->state == ENET_PEER_STATE_CONNECTED &&
            (uint32_t)(uintptr_t)p->data == player->id) {
            peer = p;
            break;
        }
    }

    if (peer == NULL)
        return;

    LevelUpPacket level_up = {
        .type = PACKET_LEVEL_UP,
        .player_id = player->id,
        .new_level = player->level
    };

    ENetPacket *packet = enet_packet_create(
        &level_up,
        sizeof(level_up),
        ENET_PACKET_FLAG_RELIABLE
    );

    if (packet != NULL && enet_peer_send(peer, 1, packet) < 0)
        enet_packet_destroy(packet);

    uint32_t spells[3];
    size_t spell_count = 0;
    spell_selection(spells, 3, &spell_count);

    SpellChoicePacket spell_choice = {
        .type = PACKET_SPELL_CHOICE,
        .player_id = player->id
    };
    memcpy(spell_choice.spells, spells, spell_count * sizeof(uint32_t));

    ENetPacket *choice_packet = enet_packet_create(
        &spell_choice,
        sizeof(spell_choice),
        ENET_PACKET_FLAG_RELIABLE
    );

    if (choice_packet != NULL && enet_peer_send(peer, 1, choice_packet) < 0)
        enet_packet_destroy(choice_packet);
}
