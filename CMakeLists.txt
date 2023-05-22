cmake_minimum_required(VERSION 3.25)
project(server C)

set(CMAKE_C_STANDARD 23)

add_executable(
        server
        main.c
        map/map.c
        vec/vec.c
        binary/binary.c
        connection/connection.c
        user/user.c
        game-server/game-server.c
        utils/utils.c
        packets/packets.c
        auth/auth.c
        globals.c
        rpc-commands/handle-create-game-server.rpc-command.c
        rpc-commands/handle-connect-to-server.rpc-command.c
        rpc-commands/handle-get-game-servers.rpc-command.c
)

if(WIN32)
    target_link_libraries(server wsock32 ws2_32)
endif()

