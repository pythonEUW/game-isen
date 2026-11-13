#include "network_client.h"

#include <stdio.h>
#include <string.h>

#include "protocol.h"

int network_client_connect(NetworkClient *nc, const char *address, uint16_t port)
{
    nc->host = enet_host_create(NULL, 1, ENET_CHANNEL_COUNT, 0, 0);
    if (nc->host == NULL)
    {
        fprintf(stderr, "ENet: impossible de creer le host client.\n");
        nc->peer = NULL;
        return -1;
    }

    ENetAddress addr;
    enet_address_set_host(&addr, address);
    addr.port = port;

    nc->peer = enet_host_connect(nc->host, &addr, ENET_CHANNEL_COUNT, 0);
    if (nc->peer == NULL)
    {
        fprintf(stderr, "ENet: enet_host_connect a echoue.\n");
        return -1;
    }

    ENetEvent event;
    if (enet_host_service(nc->host, &event, 3000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        printf("ENet: connecte au serveur %s:%d\n", address, port);
        return 0;
    }

    printf("ENet: serveur non atteignable sur %s:%d\n", address, port);
    nc->peer = NULL;
    return -1;
}

int network_client_send(NetworkClient *nc, const char *msg)
{
    if (nc->peer == NULL)
        return -1;

    ENetPacket *packet = enet_packet_create(msg, strlen(msg) + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(nc->peer, 0, packet);
    enet_host_flush(nc->host);
    return 0;
}

int network_client_poll(NetworkClient *nc)
{
    if (nc->host == NULL)
        return -1;

    ENetEvent event;
    int received = 0;
    while (enet_host_service(nc->host, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            printf("Recu du serveur : %.*s\n",
                   (int)event.packet->dataLength, (char *)event.packet->data);
            enet_packet_destroy(event.packet);
            received = 1;
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            printf("Deconnecte par le serveur.\n");
            nc->peer = NULL;
            return -1;
        default:
            break;
        }
    }
    return received;
}

void network_client_disconnect(NetworkClient *nc)
{
    if (nc->peer != NULL)
    {
        enet_peer_disconnect(nc->peer, 0);

        ENetEvent event;
        while (enet_host_service(nc->host, &event, 1000) > 0 &&
               event.type != ENET_EVENT_TYPE_DISCONNECT)
        {
        }
        nc->peer = NULL;
    }

    if (nc->host != NULL)
    {
        enet_host_destroy(nc->host);
        nc->host = NULL;
    }
}
