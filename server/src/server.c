
#include "server.h"
#include "network_server.h"
#include "world.h"

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
    World world;

    world_init(&world);

    if (network_server_init(&network) != 0)
        return 1;

    signal(SIGINT, handle_signal);

    printf("En attente de joueurs...\n");

    enet_uint32 previous = enet_time_get();
    float accumulator = 0.0f;
    const float tick = 1.0f / 60.0f;
    while (running) {
        // Plus tard :
        // player_update();
        // enemy_update();

        if (network_server_poll(&network, &world) != 0)
            break;

        enet_uint32 now = enet_time_get();
        float elapsed = (now - previous) / 1000.0f;
        previous = now;
        accumulator += elapsed > 0.1f ? 0.1f : elapsed;
        if (accumulator >= tick) {
            while (accumulator >= tick) {
                world_update(&world, tick);
                accumulator -= tick;
            }
            network_broadcast_world_state(&network, &world);
            enet_host_flush(network.host);
        }
    }

    network_server_close(&network);

    return 0;
}
