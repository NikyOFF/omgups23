//
// Created by nikyoff on 10.05.2023.
//

#ifndef PACKETS_H
#define PACKETS_H

#include <winsock2.h>

#include "../user/user.h"
#include "../auth/auth.h"
#include "../connection/connection.h"
#include "../binary/binary.h"


int sendAuthPacket(SOCKET socket, char* login, char* password);

typedef struct ClientAuthPacketStruct {
    size_t loginSize;
    char* login;
    size_t passwordSize;
    char* password;
} ClientAuthPacket;

int receiveAuthPacket(Connection* connection, ClientAuthPacket* authPacket);

int sendEndAuthPacket(Connection* connection, bool success, User* user);

typedef struct GetGameServersRpcClientPacketStruct {
    char* rpcCommand;
} GetGameServersRpcClientPacket;

typedef struct CreateGameServerRpcClientPacketStruct {
    char* rpcCommand;
    char* serverName;
} CreateGameServerRpcClientPacket;

typedef struct ConnectToGameServerRpcClientPacketStruct {
    char* rpcCommand;
    char* serverId;
} ConnectToGameServerRpcClientPacket;

#endif //PACKETS_H
