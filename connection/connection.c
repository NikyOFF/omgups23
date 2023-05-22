#include "connection.h"

Connection* Connection_constructor(SOCKET socket, ConnectionState connectionState, User* user) {
    Connection* connection = calloc(1, sizeof(Connection));

    connection->socket = socket;
    connection->connectionState = connectionState;
    connection->user = user;
    connection->lastHealthCheck = -1;

    return connection;
}