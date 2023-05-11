//
// Created by nikyoff on 10.05.2023.
//

#ifndef PACKETS_H
#define PACKETS_H

#include <stdbool.h>
#include <winsock2.h>

#include "../user/user.h"
#include "../binary/binary.h"


int sendAuthPacket(SOCKET socket, char* login, char* password);

typedef struct ServerEndAuthPacketStruct {
    bool success;
    User* user;
} ServerEndAuthPacket;

int receiveEndAuthPacket(SOCKET socket, ServerEndAuthPacket* serverEndAuthPacket);

#endif //PACKETS_H
