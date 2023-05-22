#include <stdio.h>
#include <winsock2.h>
#include "../user/user.h"
#include "../vec/vec.h"

#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

typedef unsigned char ConnectionState;
#define AUTHORIZATION_CONNECTION_STATE 0
#define AUTHORIZED_CONNECTION_STATE 1

typedef struct ConnectionStruct {
    SOCKET socket;
    ConnectionState connectionState;
    User* user;
    long long lastHealthCheck;
} Connection;

Connection* Connection_constructor(SOCKET socket, ConnectionState connectionState, User* user);

typedef vec_t(Connection*) Connection_vec_t;
#endif //SERVER_CONNECTION_H
