
cmake_minimum_required(VERSION 3.16)

project(game_isen LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(shared STATIC
    shared/src/protocol.c
)

target_include_directories(shared PUBLIC
    shared/include
)

add_executable(client
    client/src/main.c
    client/src/game.c
    client/src/network_client.c
)

target_include_directories(client PRIVATE
    client/include
)

target_link_libraries(client PRIVATE shared)

add_executable(server
    server/src/main.c
    server/src/server.c
    server/src/network_server.c
)

target_include_directories(server PRIVATE
    server/include
)

target_link_libraries(server PRIVATE shared)
