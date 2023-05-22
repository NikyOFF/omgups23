#ifndef UTILS_H_
#include "utils.h"


Connection* getConnectionBySocket(SOCKET* socket) {
    int index;
    Connection* connection;

    vec_foreach(&CONNECTIONS_VEC, connection, index) {
            if (&connection->socket == socket) {
                return connection;
            }
        }

    return NULL;
}

GameServer* getGameServerById(size_t id) {
    int index;
    GameServer* gameServer;

    vec_foreach(&GAME_SERVER_VEC, gameServer, index) {
            if (gameServer->id == id) {
                return gameServer;
            }
        }

    return NULL;
}

void printfForConnection(const Connection* connection, const char* format, ...) {
    HANDLE consoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    va_list args;
    fprintf(stdout, "[Client %llu THREAD]: ", connection->socket);
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
}

void onDisconnect(Connection* connection) {
    printfForConnection(connection, "Close connection...");

    pthread_mutex_lock(&CONNECTIONS_MUTEX);
    vec_remove(&CONNECTIONS_VEC, connection);
    pthread_mutex_unlock(&CONNECTIONS_MUTEX);

    pthread_mutex_lock(&GAME_SERVERS_MUTEX);
    int index;
    GameServer* gameServer;
    vec_foreach(&GAME_SERVER_VEC, gameServer, index) {
            if (gameServer->_owner != connection->socket) {
                continue;
            }

            vec_remove(&GAME_SERVER_VEC, gameServer);
        }
    pthread_mutex_unlock(&GAME_SERVERS_MUTEX);

    closesocket(connection->socket);
}

void* handleSocketError(Connection* connection) {
    onDisconnect(connection);
    printfForConnection(connection, "User disconnected!");

    pthread_exit(0);
    return NULL;
}

#endif //UTILS_H_