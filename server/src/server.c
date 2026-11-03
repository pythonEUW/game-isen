
#include "server.h"
#include "network_server.h"

#include <signal.h>
#include <stdio.h>

static volatile sig_atomic_t running = 1;

static void handle_signal(int signal_number)
{
    (void)signal_number;
    running = 0;
}

int server_run(void)
{
    NetworkServer network;

    if (network_server_init(&network) != 0)
        return 1;

    signal(SIGINT, handle_signal);

    printf("En attente de joueurs...\n");

    while (running) {
        if (network_server_poll(&network) != 0)
            break;

        // Plus tard :
        // world_update();
        // player_update();
        // enemy_update();
    }

    network_server_close(&network);

    return 0;
}
