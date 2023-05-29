cmake_minimum_required(VERSION 3.25)
project(work2 C)

set(CMAKE_C_STANDARD 23)

add_executable(work2 main.c client.h user/user.c user/user.h binary/binary.c binary/binary.h cursor/cursor.h cursor/cursor.c game-server/game-server.h game-server/game-server.c packets/packets.c packets/packets.h socket/socket.h socket/socket.c utils/utils.c utils/utils.h color/color.h team/team.h team/team.c unit/unit.h unit/unit.c globals.c globals.h)

if(WIN32)
    target_link_libraries(work2 wsock32 ws2_32)
endif()
