#include "game-server.h"



GameServer* GameServer_constructor(size_t id, char* serverName, SOCKET owner) {
    GameServer* gameServer = calloc(1, sizeof(GameServer));

    gameServer->id = id;
    gameServer->serverNameSize = strlen(serverName);
    gameServer->serverName = serverName;

    gameServer->_owner = owner;
    pthread_mutex_init(&gameServer->_serverMutex, NULL);
    gameServer->_connectionRequest = PTHREAD_COND_INITIALIZER;

    return gameServer;
}


void Binary_writeGameServer(Binary* binary, GameServer* gameServer) {
    Binary_writeSizeT(binary, gameServer->id);
    Binary_writeString(binary, gameServer->serverName);
}

GameServer* Binary_readGameServer(Binary* binary) {
    GameServer* gameServer = calloc(1, sizeof(GameServer));

    Binary_readSizeT(binary, &gameServer->id);
    gameServer->serverName = Binary_readString(binary, &gameServer->serverNameSize);

    return gameServer;
}
