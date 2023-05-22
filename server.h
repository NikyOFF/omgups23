#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

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

#include "binary/binary.h"
#include "game-server/game-server.h"
#include "connection/connection.h"
#include "user/user.h"
#include "auth/auth.h"

#include "rpc-commands/rpc-commands.h"

#include "globals.h"
#include "utils/utils.h"
#include "packets/packets.h"



#endif //SERVER_SERVER_H