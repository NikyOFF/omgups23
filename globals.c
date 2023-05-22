#include "globals.h"

SOCKET SERVER_SOCKET;

pthread_mutex_t GENERAL_AUTH_MUTEX;

pthread_mutex_t CONNECTIONS_MUTEX;

Connection_vec_t CONNECTIONS_VEC;

pthread_mutex_t GAME_SERVERS_MUTEX;

GameServer_vec_t GAME_SERVER_VEC;

