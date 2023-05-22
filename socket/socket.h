#ifndef SOCKET_H
#define SOCKET_H
#include <winsock2.h>
#include "../binary/binary.h"

#define SERVER_SOCKET_PING_PACKET_MESSAGE "SERVER_PING"
#define SERVER_SOCKET_PING_PACKET_MESSAGE_SIZE 12
#define SERVER_SOCKET_PING_PACKET_SIZE 20

#define CLIENT_SOCKET_PING_PACKET_MESSAGE "CLIENT_PONG"
#define CLIENT_SOCKET_PING_PACKET_SIZE 20

int receiveServerPacket(SOCKET socket, Binary* serverPacket, int flags);
#endif //SOCKET_H
