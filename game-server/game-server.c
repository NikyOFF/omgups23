#include "game-server.h"



GameServer* GameServer_constructor(size_t id, char* serverName) {
    GameServer* gameServer = calloc(1, sizeof(GameServer));

    gameServer->id = id;
    gameServer->serverNameSize = strlen(serverName);
    gameServer->serverName = serverName;

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

int rpcCreateGameServer(SOCKET socket, char* serverName, GameServer* outGameServer) {
    int iResult;

    Binary binary = *Binary_constructor(512);

    Binary_writeString(&binary, "rpc:create_game_server");
    Binary_writeString(&binary, serverName);

    iResult = send(socket, binary.buffer, Binary_getSize(&binary), 0);

    if (iResult == -1) {
        return 2;
    }

    if (iResult < 1) {
        return 1;
    }

    Sleep(100);
    Binary_clear(&binary);

    iResult = recv(socket, binary.buffer, binary.bufferSize, 0);

    if (iResult == -1) {
        return 2;
    }

    if (iResult < 1) {
        return 1;
    }

    Binary_readInt(&binary, &iResult);

    if (iResult != 0) {
        return 1;
    }

    GameServer gameServer = *Binary_readGameServer(&binary);

    memcpy(outGameServer, &gameServer, sizeof(gameServer));

    printf("Create new game server \"%s\" (%zu)\n", gameServer.serverName, gameServer.id);
    printf("Waiting player...\n");
    Binary_clear(&binary);

    iResult = recv(socket, binary.buffer, binary.bufferSize, 0);

    if (iResult == -1) {
        return 2;
    }

    if (iResult < 1) {
        return 1;
    }

    Binary_readInt(&binary, &iResult);

    if (iResult != 0) {
        return 1;
    }

    Binary_clear(&binary);
    iResult = recv(socket, binary.buffer, binary.bufferSize, 0);
    User* user = Binary_readUser(&binary);

    if (iResult == -1) {
        return 2;
    }

    if (iResult < 1) {
        return 1;
    }

    printf("User \"%s\" join to server!\n", user->login);

    free(binary.buffer);
    return 0;
}

int rpcGetGameServers(SOCKET socket, size_t* outSize, GameServer*** outGameServers) {
    int iResult;

    Binary binary = *Binary_constructor(512);
    Binary_writeString(&binary, "rpc:get_game_servers");

    iResult = send(socket, binary.buffer, Binary_getSize(&binary), 0);

    if (iResult == -1) {
        return 2;
    }

    Binary_clear(&binary);
    iResult = recv(socket, binary.buffer, binary.bufferSize, 0);

    if (iResult == -1) {
        return 2;
    }

    Binary_readSizeT(&binary, outSize);

    GameServer** gameServers = calloc(*outSize, sizeof(GameServer));

    for (size_t index = 0; index < *outSize; index++) {
        gameServers[index] = Binary_readGameServer(&binary);
    }

    *outGameServers = gameServers;

    return -1;
}

int rpcConnectionToServer(SOCKET socket, size_t serverId) {
    int iResult;

    Binary binary = *Binary_constructor(512);
    Binary_writeString(&binary, "rpc:connect_to_server");
    Binary_writeSizeT(&binary, serverId);

    iResult = send(socket, binary.buffer, Binary_getSize(&binary), 0);

    if (iResult == -1) {
        return 2;
    }

    Binary_clear(&binary);
    iResult = recv(socket, binary.buffer, binary.bufferSize, 0);

    if (iResult == -1) {
        return 2;
    }

    Binary_readInt(&binary, &iResult);

    if (iResult != 0) {
        return 1;
    }

    return 0;
}