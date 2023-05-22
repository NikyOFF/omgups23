cmake_minimum_required(VERSION 3.25)
project(work2 C)

set(CMAKE_C_STANDARD 23)

add_executable(work2 main.c ed25519/add_scalar.c ed25519/ed25519.h ed25519/fe.c ed25519/fe.h ed25519/fixedint.h ed25519/ge.c ed25519/ge.h ed25519/key_exchange.c ed25519/keypair.c ed25519/precomp_data.h ed25519/sc.c ed25519/sc.h ed25519/seed.c ed25519/sha512.c ed25519/sha512.h ed25519/sign.c ed25519/verify.c client.h user/user.c user/user.h binary/binary.c binary/binary.h cursor/cursor.h cursor/cursor.c game-server/game-server.h game-server/game-server.c packets/packets.c packets/packets.h socket/socket.h socket/socket.c)

if(WIN32)
    target_link_libraries(work2 wsock32 ws2_32)
endif()
