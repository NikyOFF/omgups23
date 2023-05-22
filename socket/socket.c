#include "socket.h"

int receiveServerPacket(SOCKET socket, Binary* serverPacket, int flags) {
    size_t defaultReadIndex = serverPacket->readIndex;

    int recvResult;
    bool flag = true;

    while(flag == true) {
        Binary_clear(serverPacket);
        recvResult = recv(socket, serverPacket->buffer, serverPacket->bufferSize, flags);

        if (recvResult == -1) {
            return -1;
        }

        if (recvResult == SERVER_SOCKET_PING_PACKET_SIZE) {
            size_t strSize;
            char* str = Binary_readString(serverPacket, &strSize);

            if (strSize == SERVER_SOCKET_PING_PACKET_MESSAGE_SIZE && strcmp(str, SERVER_SOCKET_PING_PACKET_MESSAGE) == 0) {
                Binary clientPacket = *Binary_constructor(CLIENT_SOCKET_PING_PACKET_SIZE);
                Binary_writeString(&clientPacket, CLIENT_SOCKET_PING_PACKET_MESSAGE);

                int iResult = send(socket, clientPacket.buffer, CLIENT_SOCKET_PING_PACKET_SIZE, 0);

                Sleep(100);
                free(clientPacket.buffer);

                if (iResult == -1) {
                    return -1;
                }

                continue;
            }
        }

        flag = false;
    }

    serverPacket->readIndex = defaultReadIndex;
    return recvResult;
}
