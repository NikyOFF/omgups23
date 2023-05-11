#include "packets.h"

int sendAuthPacket(SOCKET socket, char* login, char* password) {
    Binary* clientPacket = Binary_constructor(512);
    Binary_writeString(clientPacket, login);
    Binary_writeString(clientPacket, password);

    int sendResult = send(socket, clientPacket->buffer, clientPacket->writeIndex, 0);

    Binary_deconstructor(clientPacket);

    return sendResult;
}

int receiveAuthPacket(Connection* connection, ClientAuthPacket* authPacket) {
    Binary* serverPacket = Binary_constructor(512);

    int recvResult = recv(connection->socket, serverPacket->buffer, serverPacket->bufferSize, 0);

    if (recvResult == SOCKET_ERROR) {
        return recvResult;
    }

    authPacket->login = Binary_readString(serverPacket, &authPacket->loginSize);
    authPacket->password = Binary_readString(serverPacket, &authPacket->passwordSize);

    Binary_deconstructor(serverPacket);

    return recvResult;
}

int sendEndAuthPacket(Connection* connection, bool success, User* user) {
    Binary* serverPacket = Binary_constructor(512);
    Binary_writeBool(serverPacket, success);

    if (success == true) {
        Binary_writeUser(serverPacket, user);
    }

    int sendResult = send(connection->socket, serverPacket->buffer, serverPacket->writeIndex, 0);

    Binary_deconstructor(serverPacket);
    return sendResult;
}