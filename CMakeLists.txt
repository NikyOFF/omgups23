cmake_minimum_required(VERSION 3.25)
project(work1 C)

set(CMAKE_C_STANDARD 23)

add_executable(work1 main.c server.h ed25519/add_scalar.c ed25519/fe.c ed25519/fe.h ed25519/fixedint.h ed25519/ge.c ed25519/ge.h ed25519/key_exchange.c ed25519/keypair.c ed25519/precomp_data.h ed25519/sc.c ed25519/sc.h ed25519/seed.c ed25519/sha512.c ed25519/sha512.h ed25519/sign.c ed25519/verify.c auth/auth.h user/user.h auth/auth.c user/user.c connection/connection.h connection/connection.c binary/binary.h binary/binary.c vec/vec.h vec/vec.c game-server/game-server.c game-server/game-server.h packets/packets.c packets/packets.h)


if(WIN32)
    target_link_libraries(work1 wsock32 ws2_32)
endif()

