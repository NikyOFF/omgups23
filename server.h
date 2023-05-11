#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include <direct.h>
#include <conio.h>
#include <pthread.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>

#include "user/user.h"
#include "auth/auth.h"
#include "connection/connection.h"
#include "binary/binary.h"
#include "game-server/game-server.h"

#include "packets/packets.h"


#include "ed25519/ed25519.h"

#define DEFAULT_PORT 25565
#define CONNECTIONS_CHECK_DELAY_IN_SECONDS 10

void printfForConnection(const Connection* connection, const char* format, ...);