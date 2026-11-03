
cmake_minimum_required(VERSION 3.16)

project(game_isen LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Pour l'autocompletion dans Zed (clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Recherche des bibliotheques
find_package(PkgConfig REQUIRED)

pkg_check_modules(ENET REQUIRED IMPORTED_TARGET libenet)
pkg_check_modules(RAYLIB REQUIRED IMPORTED_TARGET raylib)

# ==========================================
# Shared - Code commun
# ==========================================

add_library(shared STATIC
    shared/src/protocol.c
)

target_include_directories(shared PUBLIC
    shared/include
)

# ==========================================
# Client - raylib + ENet
# ==========================================

add_executable(client
    client/src/main.c
    client/src/game.c
    client/src/network_client.c
)

target_include_directories(client PRIVATE
    client/include
)

target_link_libraries(client PRIVATE
    shared
    PkgConfig::RAYLIB
    PkgConfig::ENET
)

# ==========================================
# Serveur - ENet
# ==========================================

add_executable(server
    server/src/main.c
    server/src/server.c
    server/src/network_server.c
)

target_include_directories(server PRIVATE
    server/include
)

target_link_libraries(server PRIVATE
    shared
    PkgConfig::ENET
)
