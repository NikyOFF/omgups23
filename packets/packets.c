#include "packets.h"

int sendAuthPacket(SOCKET socket, char* login, char* password) {
    Binary* clientPacket = Binary_constructor(512);
    Binary_writeString(clientPacket, login);
    Binary_writeString(clientPacket, password);

    int sendResult = send(socket, clientPacket->buffer, clientPacket->writeIndex, 0);

    Binary_deconstructor(clientPacket);

    return sendResult;
}

int receiveEndAuthPacket(SOCKET socket, ServerEndAuthPacket* serverEndAuthPacket) {
    Binary* serverPacket = Binary_constructor(512);

    int recvResult = recv(socket, serverPacket->buffer, serverPacket->bufferSize, 0);

    if (recvResult == SOCKET_ERROR) {
        return recvResult;
    }

    Binary_readBool(serverPacket, &serverEndAuthPacket->success);

    if (serverEndAuthPacket->success == true) {
        serverEndAuthPacket->user = Binary_readUser(serverPacket);
    }
    else {
        serverEndAuthPacket->user = NULL;
    }

    Binary_deconstructor(serverPacket);

    return recvResult;
}