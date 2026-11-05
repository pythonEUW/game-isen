#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include <enet/enet.h>

typedef struct
{
    ENetHost *host;
    ENetPeer *peer; /* NULL si pas connecte */
} NetworkClient;

/* Cree le host et tente de se connecter au serveur.
   Retourne 0 si connecte, -1 sinon (le host est conserve pour reessayer). */
int network_client_connect(NetworkClient *nc, const char *address, uint16_t port);

/* Envoie un paquet texte sur le canal 0. */
int network_client_send(NetworkClient *nc, const char *msg);

/* A appeler a chaque frame : traite les evenements ENet (reception, deconnexion).
   Retourne 1 si un paquet a ete recu, 0 sinon, -1 si deconnecte. */
int network_client_poll(NetworkClient *nc);

/* Deconnecte proprement et detruit le host. */
void network_client_disconnect(NetworkClient *nc);

#endif
