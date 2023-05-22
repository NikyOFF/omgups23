#ifndef UTILS_H
#define UTILS_H
#include "../globals.h"

void printfForConnection(const Connection* connection, const char* format, ...);

Connection* getConnectionBySocket(SOCKET* socket);
GameServer* getGameServerById(size_t id);

void onDisconnect(Connection* connection);
void* handleSocketError(Connection* connection);

#endif //UTILS_H
