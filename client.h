#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <conio.h>

#include "globals.h"
#include "utils/utils.h"
#include "socket/socket.h"
#include "cursor/cursor.h"
#include "binary/binary.h"
#include "user/user.h"
#include "game-server/game-server.h"
#include "packets/packets.h"
#include "unit/unit.h"
#include "team/team.h"


#define DEFAULT_PORT 25565

#define DEFAULT_NUMBER_OF_TEAMS 2
#define DEFAULT_NUMBER_OF_UNITS_IN_TEAMS 2

//#region scene&menus
#define EXIT_SCENE 0

#define MAIN_MENU_SCENE 1

#define MAIN_MENU_MENU 10
#define START_NEW_GAME_MENU 11
#define SINGLEPLAYER_MENU 110
#define MULTIPLAYER_MENU 111
#define SELECT_SERVER_MENU 1110
#define STATISTICS_MENU 12


#define SERVER_SCENE 2

#define PVE_SINGLEPLAYER_SCENE 20

#define PVP_SINGLEPLAYER_SCENE 30
//#endregion


//#region bots
#define BOT_DELAY false

#define IF_BOT_DELAY \
if (BOT_DELAY)

#define BOT_BRAIN_SPEED_MULTIPLIER BOT_DELAY ? 1 : 0
#define DELAY_AFTER_BOT_SELECT BOT_DELAY ? 500 : 0
//#endregion


//#region process auth
#define INVALID_SOCKET_PROCESS_AUTH_RESULT 0
#define INVALID_PASSWORD_PROCESS_AUTH_RESULT 1
#define SUCCESS_PROCESS_AUTH_RESULT 2
int processAuth(User* user);
//#endregion
