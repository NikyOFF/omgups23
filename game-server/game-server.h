#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <memory.h>
#include <pthread.h>

#include "../binary/binary.h"
#include "../vec/vec.h"


typedef struct GameServerStruct {
    size_t id;
    size_t serverNameSize;
    char* serverName;

    SOCKET _owner;
    pthread_mutex_t _serverMutex;
    pthread_cond_t _connectionRequest;
} GameServer;


GameServer* GameServer_constructor(size_t id, char* serverName, SOCKET owner);


void Binary_writeGameServer(Binary* binary, GameServer* gameServer);
GameServer* Binary_readGameServer(Binary* binary);

typedef vec_t(GameServer*) GameServer_vec_t;

#endif //GAME_SERVER_H
