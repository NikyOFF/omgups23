#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <memory.h>

#include "../binary/binary.h"
#include "../user/user.h"

#ifndef GAME_SERVER_H
#define GAME_SERVER_H

typedef struct GameServerStruct {
    size_t id;
    size_t serverNameSize;
    char* serverName;
} GameServer;

GameServer* GameServer_constructor(size_t id, char* serverName);


void Binary_writeGameServer(Binary* binary, GameServer* gameServer);
GameServer* Binary_readGameServer(Binary* binary);

int rpcCreateGameServer(SOCKET socket, char* serverName, GameServer* outGameServer);
int rpcGetGameServers(SOCKET socket, size_t* outSize, GameServer** outGameServers);
int rpcConnectionToServer(SOCKET socket, size_t serverId);

#endif //GAME_SERVER_H
