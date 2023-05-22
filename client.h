#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include <pthread.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <conio.h>
#include <wincon.h>

#include "socket/socket.h"
#include "cursor/cursor.h"
#include "binary/binary.h"
#include "user/user.h"
#include "game-server/game-server.h"

#include "packets/packets.h"

#include "ed25519/ed25519.h"

#define DEFAULT_PORT 25565



#define INVALID_SOCKET_PROCESS_AUTH_RESULT 0
#define INVALID_PASSWORD_PROCESS_AUTH_RESULT 1
#define SUCCESS_PROCESS_AUTH_RESULT 2
int processAuth(User* user);
