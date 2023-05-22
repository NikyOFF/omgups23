#ifndef SERVER_GLOBALS_H
#define SERVER_GLOBALS_H

#include <pthread.h>
#include <winsock2.h>

#include "connection/connection.h"
#include "game-server/game-server.h"


#define DEFAULT_PORT 25565
#define CONNECTIONS_CHECK_DELAY_IN_SECONDS 10

extern SOCKET SERVER_SOCKET;

extern pthread_mutex_t GENERAL_AUTH_MUTEX;

extern pthread_mutex_t CONNECTIONS_MUTEX;

extern Connection_vec_t CONNECTIONS_VEC;

extern pthread_mutex_t GAME_SERVERS_MUTEX;

extern GameServer_vec_t GAME_SERVER_VEC;

#endif //SERVER_GLOBALS_H
