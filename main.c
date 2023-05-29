#include "client.h"

SOCKET CLIENT_SOCKET = INVALID_SOCKET;
size_t CURRENT_SCENE = MAIN_MENU_SCENE;
size_t CURRENT_MENU = MAIN_MENU_MENU;
User CURRENT_USER;
bool IS_SERVER_OWNER = false;
GameServer CURRENT_GAME_SERVER;


//#region network
int connectToServer() {
    int iResult;
    WSADATA wsData;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsData);

    if (iResult != 0) {
        printf("[main] Failed to initialize socket\n");

        Sleep(1000);
        MOVE_CURSOR_UP(1);

        return 1;
    }

    CLIENT_SOCKET = socket(AF_INET, SOCK_STREAM, 0);

    if (CLIENT_SOCKET == INVALID_SOCKET) {
        printf("[main] Failed to create socket\n");

        Sleep(1000);
        MOVE_CURSOR_UP(1);

        return 1;
    }

    printf("[main] Server socket initialization is OK\n");

    struct in_addr ip_to_num;

    iResult = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);

    if (iResult <= 0) {
        printf("[main] Error in IP translation to special numeric format\n");

        Sleep(1000);
        MOVE_CURSOR_UP(1);

        return 1;
    }

    struct sockaddr_in serverInfo;
    ZeroMemory(&serverInfo, sizeof(serverInfo));

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr = ip_to_num;
    serverInfo.sin_port = htons(DEFAULT_PORT);

    iResult = connect(CLIENT_SOCKET, (struct sockaddr*)&serverInfo, sizeof(serverInfo));

    if (iResult != 0) {
        printf("[main] Connection to Server is FAILED\n");

        Sleep(1000);
        MOVE_CURSOR_UP(1);

        return 1;
    }

    printf("[main] Connection established SUCCESSFULLY. Ready to send a message to Server\n\n");

    Sleep(1000);
    MOVE_CURSOR_UP(2);

    return 0;
}

int processAuth(User* outUser) {
    int iResult;
    char loginBuffer[64];
    char passwordBuffer[64];

    printf("login: ");
    scanf("%socket", loginBuffer);

    printf("password: ");
    scanf("%socket", passwordBuffer);

    iResult = sendAuthPacket(CLIENT_SOCKET, loginBuffer, passwordBuffer);

    if (iResult == INVALID_SOCKET) {
        return INVALID_SOCKET_PROCESS_AUTH_RESULT;
    }

    ServerEndAuthPacket serverEndAuthPacket;
    iResult = receiveEndAuthPacket(CLIENT_SOCKET, &serverEndAuthPacket);

    if (iResult == INVALID_SOCKET) {
        return INVALID_SOCKET_PROCESS_AUTH_RESULT;
    }

    if (serverEndAuthPacket.success != true) {
        return INVALID_PASSWORD_PROCESS_AUTH_RESULT;
    }

    memcpy(outUser, serverEndAuthPacket.user, sizeof(User));
    free(serverEndAuthPacket.user);

    return SUCCESS_PROCESS_AUTH_RESULT;
}
//#endregion


//#region ui
void selectServerIteration() {
    size_t gameServersSize;
    GameServer** gameServers = NULL;
    int iResult = rpcGetGameServers(CLIENT_SOCKET, &gameServersSize, &gameServers);

    if (iResult == 2) {
        printf("Error\n");
        CURRENT_SCENE = EXIT_SCENE;
        return;
    }

    if (iResult == 1) {
        printf("Cannot get game servers\n");
        Sleep(3000);
        MOVE_CURSOR_UP(1llu);
        ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

        CURRENT_MENU = START_NEW_GAME_MENU;
        return;
    }

    if (gameServersSize == 0) {
        printf("No available servers\n");
        Sleep(3000);
        MOVE_CURSOR_UP(1llu);
        ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

        CURRENT_MENU = START_NEW_GAME_MENU;
        return;
    }

    size_t itemsSize = gameServersSize + 1;
    char** items = calloc(itemsSize, (sizeof(char*)));

    for (size_t index = 0; index < gameServersSize; index++) {
        char* item = calloc(1, sizeof(char) * 100);
        sprintf(item, "server \"%s\" (id:%zu)", gameServers[index]->serverName, gameServers[index]->id);
        items[index] = item;
    }

    items[gameServersSize] = "back";
    size_t selectedIndex = 0;

    while(1) {
        printSelectMenu(items, itemsSize, selectedIndex);
        MOVE_CURSOR_UP(itemsSize);

        switch(getch()) {
            case 'w':
            case 72:
                selectedIndex = selectedIndex == 0 ? gameServersSize : selectedIndex - 1;
                break;
            case 's':
            case 80:
                selectedIndex = selectedIndex + 1 >= itemsSize ? 0 : selectedIndex + 1;
                break;
            case '\n':
            case 13:
            case 100:
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                if (selectedIndex < gameServersSize) {
                    memcpy(&CURRENT_GAME_SERVER, gameServers[selectedIndex], sizeof(GameServer));
                    CURRENT_SCENE = SERVER_SCENE;
                }
                else if (selectedIndex == gameServersSize) {
                    CURRENT_MENU = MULTIPLAYER_MENU;
                }

                free(gameServers);
                free(items);

                return;
        }
    }
}

void multiplayerMenuIteration() {
    char* items[3] = {"create server", "select server", "back"};
    size_t itemsSize = 3;
    size_t selectedIndex = 0;

    while(1) {
        printSelectMenu(items, itemsSize, selectedIndex);
        MOVE_CURSOR_UP(itemsSize);

        switch(getch()) {
            case 'w':
            case 72:
                selectedIndex = selectedIndex == 0 ? itemsSize - 1 : selectedIndex - 1;
                break;
            case 's':
            case 80:
                selectedIndex = selectedIndex + 1 >= itemsSize ? 0 : selectedIndex + 1;
                break;
            case '\n':
            case 13:
            case 100:
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                if (selectedIndex == 0) {
                    char serverName[50];
                    printf("server name: ");
                    scanf("%s", serverName);

                    GameServer gameServer;
                    int iResult = rpcCreateGameServer(CLIENT_SOCKET, serverName, &gameServer);

                    if (iResult == 2) {
                        CURRENT_SCENE = EXIT_SCENE;
                        return;
                    }

                    if (iResult == 1) {
                        printf("Cannot create server, exit to main menu...\n");
                        Sleep(5000);
                        CURRENT_MENU = MAIN_MENU_MENU;
                        return;
                    }

                    IS_SERVER_OWNER = true;
                    memcpy(&CURRENT_GAME_SERVER, &gameServer, sizeof(GameServer));
                    CURRENT_SCENE = SERVER_SCENE;
                    return;
                }

                if (selectedIndex == 1) {
                    CURRENT_MENU = SELECT_SERVER_MENU;
                }

                if (selectedIndex == 2) {
                    CURRENT_MENU = START_NEW_GAME_MENU;
                }

                return;
        }
    }
}

void singleplayerMenuIteration() {
    char* items[3] = {"play vs environment", "player vs player", "back"};
    size_t itemsSize = 3;
    size_t selectedIndex = 0;

    while(1) {
        printSelectMenu(items, itemsSize, selectedIndex);
        MOVE_CURSOR_UP(itemsSize);

        switch(getch()) {
            case 'w':
            case 72:
                selectedIndex = selectedIndex == 0 ? itemsSize - 1 : selectedIndex - 1;
                break;
            case 's':
            case 80:
                selectedIndex = selectedIndex + 1 >= itemsSize ? 0 : selectedIndex + 1;
                break;
            case '\n':
            case 13:
            case 100:
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                if (selectedIndex == 0) {
                    CURRENT_SCENE = PVE_SINGLEPLAYER_SCENE;
                }
                else if (selectedIndex == 1) {
                    CURRENT_SCENE = PVP_SINGLEPLAYER_SCENE;
                }
                else if (selectedIndex == 2) {
                    CURRENT_MENU = START_NEW_GAME_MENU;
                }

                return;
        }
    }
}

void startNewGameMenuIteration() {
    char* items[3] = {"singlplayer", "multiplayer", "back"};
    size_t itemsSize = 3;
    size_t selectedIndex = 0;

    while(1) {
        printSelectMenu(items, itemsSize, selectedIndex);
        MOVE_CURSOR_UP(itemsSize);

        switch(getch()) {
            case 'w':
            case 72:
                selectedIndex = selectedIndex == 0 ? itemsSize - 1 : selectedIndex - 1;
                break;
            case 's':
            case 80:
                selectedIndex = selectedIndex + 1 >= itemsSize ? 0 : selectedIndex + 1;
                break;
            case '\n':
            case 13:
            case 100:
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                if (selectedIndex == 0) {
                    CURRENT_MENU = SINGLEPLAYER_MENU;
                }
                else if (selectedIndex == 1) {
                    CURRENT_MENU = MULTIPLAYER_MENU;
                }
                else if (selectedIndex == 2) {
                    CURRENT_MENU = MAIN_MENU_MENU;
                }

                return;
        }
    }
}

void statisticsMenuIteration() {
    char winsItem[20];
    sprintf(winsItem, "wins: %zu", CURRENT_USER.wins);

    char defeatsItem[20];
    sprintf(defeatsItem, "defeats: %zu", CURRENT_USER.defeats);

    char* items[3] = {winsItem, defeatsItem, "back"};
    size_t itemsSize = 3;
    size_t selectedIndex = 0;

    while(1) {
        printSelectMenu(items, itemsSize, selectedIndex);
        MOVE_CURSOR_UP(itemsSize);

        switch(getch()) {
            case 'w':
            case 72:
                selectedIndex = selectedIndex == 0 ? itemsSize - 1 : selectedIndex - 1;
                break;
            case 's':
            case 80:
                selectedIndex = selectedIndex + 1 >= itemsSize ? 0 : selectedIndex + 1;
                break;
            case '\n':
            case 13:
            case 100:
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                if (selectedIndex == 2) {
                    CURRENT_MENU = MAIN_MENU_MENU;
                    return;
                }
        }
    }
}

void mainMenuIteration() {
    char* items[3] = {"New game", "Statistic", "Exit"};
    size_t itemsSize = 3;
    size_t selectedIndex = 0;

    while(1) {
        printSelectMenu(items, itemsSize, selectedIndex);
        MOVE_CURSOR_UP(itemsSize);

        switch(getch()) {
            case 'w':
            case 72:
                selectedIndex = selectedIndex == 0 ? itemsSize - 1 : selectedIndex - 1;
                break;
            case 's':
            case 80:
                selectedIndex = selectedIndex + 1 >= itemsSize ? 0 : selectedIndex + 1;
                break;
            case '\n':
            case 13:
            case 100:
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                if (selectedIndex == 0) {
                    CURRENT_MENU = START_NEW_GAME_MENU;
                }

                if (selectedIndex == 1) {
                    CURRENT_MENU = STATISTICS_MENU;
                }

                if (selectedIndex == 2) {
                    CURRENT_SCENE = EXIT_SCENE;
                }

                return;
        }
    }
}

void mainMenuSceneIteration() {
    switch (CURRENT_MENU) {
        case MAIN_MENU_MENU:
            mainMenuIteration();
            break;
        case START_NEW_GAME_MENU:
            startNewGameMenuIteration();
            break;
        case SINGLEPLAYER_MENU:
            singleplayerMenuIteration();
            break;
        case MULTIPLAYER_MENU:
            multiplayerMenuIteration();
            break;
        case SELECT_SERVER_MENU:
            selectServerIteration();
            break;
        case STATISTICS_MENU:
            statisticsMenuIteration();
            break;
    }
}
//#endregion

//#region gameplay
#define MAX_TEAM_NAME_LENGTH 50
#define DEFAULT_NUMBER_OF_TEAMS 2

void pvpSingleplayerGameLoop() {
    Team** teams;
    size_t numberOfTeams;
    size_t numberOfUnitsInTeams;

    #define MAX_RESULT_LENGTH 10
    char result[MAX_RESULT_LENGTH];

    while (true) {
        printf("Enter the number of teams (by default %llu): ", DEFAULT_NUMBER_OF_TEAMS);
        printfWithRGBColor(&DEFEAT_TEAM_UNIT_COLOR, NULL, "2");
        MOVE_CURSOR_LEFT(1);

        scanString(result, MAX_RESULT_LENGTH);

        if (result[0] == '\0') {
            numberOfTeams = DEFAULT_NUMBER_OF_TEAMS;
            break;
        }
        else if (isCharNumber(result) && !isNegativeCharNumber(result)) {
            numberOfTeams = charNumberToDecimal(result);

            if (numberOfTeams > 1) {
                break;
            }
        }
        else {
            MOVE_CURSOR_UP_HOME(1);
            ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
            printf("Invalid value\n");
            Sleep(2000);
            MOVE_CURSOR_UP_HOME(1);
            ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

            numberOfTeams = 0;
        }
    }

    while (true) {
        printf("Enter the number of units in teams (by default %llu): ", DEFAULT_NUMBER_OF_UNITS_IN_TEAMS);
        printfWithRGBColor(&DEFEAT_TEAM_UNIT_COLOR, NULL, "1");
        MOVE_CURSOR_LEFT(1);

        scanString(result, MAX_RESULT_LENGTH);

        if (result[0] == '\0') {
            numberOfUnitsInTeams = DEFAULT_NUMBER_OF_UNITS_IN_TEAMS;
            break;
        }
        else if (isCharNumber(result) && !isNegativeCharNumber(result)) {
            numberOfUnitsInTeams = charNumberToDecimal(result);

            if (numberOfUnitsInTeams > 0) {
                break;
            }
        }
        else {
            MOVE_CURSOR_UP_HOME(1);
            ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
            printf("Invalid value\n");
            Sleep(2000);
            MOVE_CURSOR_UP_HOME(1);
            ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

            numberOfUnitsInTeams = 0;
        }
    }

    MOVE_CURSOR_UP(2);
    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

    teams = calloc(numberOfTeams, sizeof(Team*));

    for (size_t index = 0; index < numberOfTeams; index++) {
        char name[MAX_TEAM_NAME_LENGTH];

        while (true) {
            printf("Enter the name for team #%llu: ", index + 1);

            scanString(name, MAX_TEAM_NAME_LENGTH);

            if (strlen(name) > 0) {
                break;
            }

            MOVE_CURSOR_UP_HOME(1);
            ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
            printf("Invalid value\n");
            Sleep(2000);
            MOVE_CURSOR_UP_HOME(1);
            ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
        }

        teams[index] = Team_constructor(name, numberOfUnitsInTeams);
    }

    Sleep(1000);
    MOVE_CURSOR_UP(numberOfTeams);
    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

    size_t currentTeamIndex = 0;
    size_t currentLastTeamIndex = 0;
    size_t teamsAlive = numberOfTeams;

    size_t currentUnitIndex = -1;
    size_t enemyTeamIndex = -1;
    size_t enemyUnitIndex = -1;

    INITIALIZE_SELECT_MENU(
        6,
        0,
        "Select unit for attack",
        "Select team to attack",
        "Select unit to attack",
        "Attack enemy",
        "Skip round",
        "Stop game"
    );

    bool teamsDirty = true;

    while(teamsAlive > 1) {
        if (Team_isDefeat(teams[currentTeamIndex])) {
            currentTeamIndex = (currentTeamIndex + 1) % numberOfTeams;
        }

        if (teamsDirty == true) {
            teamsDirty = false;
            printf("Round for team \"%s\" (#%llu)\n", teams[currentTeamIndex]->name, currentTeamIndex + 1);

            printTeams(
                teams,
                numberOfTeams,
                numberOfUnitsInTeams,
                currentTeamIndex,
                currentUnitIndex,
                enemyTeamIndex,
                enemyUnitIndex
            );
        }

        bool lockedItems[6] = {
            false,
            false,
            enemyTeamIndex == -1,
            currentUnitIndex == -1 || enemyTeamIndex == -1 || enemyUnitIndex == -1,
            false,
            false
        };

        //print menu
        SelectMenuOptions options = {
            _menuItemsSize,
            _menuItems,
            lockedItems,
            SELECTED_COLOR,
            SELECTED_BACKGROUND_COLOR,
            LOCKED_COLOR,
            LOCKED_BACKGROUND_COLOR,
            SELECTED_LOCKED_COLOR,
            SELECTED_LOCKED_BACKGROUND_COLOR
        };

        SELECT_MENU_2_BEHAVIOR(options) {
            ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

            if (_menuSelectedIndex == 0) {
                size_t internalCurrentUnitIndex = currentUnitIndex;

                printf("Enter index (0 <= index < %llu) of unit for attack: ", numberOfUnitsInTeams);
                scanf("%llu", &internalCurrentUnitIndex);

                if (internalCurrentUnitIndex >= numberOfUnitsInTeams) {
                    MOVE_CURSOR_UP(1);
                    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                    printf("Invalid unit index\n");
                    Sleep(2000);
                    MOVE_CURSOR_UP(1);
                }
                else if (teams[currentTeamIndex]->units[internalCurrentUnitIndex]->power == 0) {
                    MOVE_CURSOR_UP(1);
                    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                    printf("Invalid unit (%llu) because is have zero power\n", internalCurrentUnitIndex);
                    Sleep(2000);
                    MOVE_CURSOR_UP(1);
                }
                else {
                    currentUnitIndex = internalCurrentUnitIndex;
                    teamsDirty = true;
                    MOVE_CURSOR_UP((numberOfTeams * 2) + 4);
                }

                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
            }

            if (_menuSelectedIndex == 1) {
                size_t internalEnemyTeamIndex = enemyTeamIndex;

                printf("Enter index (0 <= index < %llu) of enemy team: ", numberOfTeams);
                scanf("%llu", &internalEnemyTeamIndex);

                if (internalEnemyTeamIndex >= numberOfTeams) {
                    MOVE_CURSOR_UP(1);
                    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                    printf("Invalid enemy team index\n");
                    Sleep(2000);
                    MOVE_CURSOR_UP(1);
                }
                else if (currentTeamIndex == internalEnemyTeamIndex) {
                    MOVE_CURSOR_UP(1);
                    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                    printf("Invalid team (%llu) because is your team man....\n", internalEnemyTeamIndex);
                    Sleep(2000);
                    MOVE_CURSOR_UP(1);
                }
                else {
                    enemyTeamIndex = internalEnemyTeamIndex;
                    enemyUnitIndex = -1;
                    teamsDirty = true;
                    MOVE_CURSOR_UP((numberOfTeams * 2) + 4);
                }

                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
            }

            if (_menuSelectedIndex == 2 && lockedItems[2] == false) {
                size_t internalEnemyUnitIndex = enemyUnitIndex;

                printf("Enter index (0 <= index < %llu) of unit to attack: ", numberOfUnitsInTeams);
                scanf("%llu", &internalEnemyUnitIndex);

                if (internalEnemyUnitIndex >= numberOfUnitsInTeams) {
                    MOVE_CURSOR_UP(1);
                    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                    printf("Invalid enemy unit index\n");
                    Sleep(2000);
                    MOVE_CURSOR_UP(1);
                }
                else if (teams[enemyTeamIndex]->units[internalEnemyUnitIndex]->power == 0) {
                    MOVE_CURSOR_UP(1);
                    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                    printf("Invalid unit (%llu) because is have zero power\n", internalEnemyUnitIndex);
                    Sleep(2000);
                    MOVE_CURSOR_UP(1);
                }
                else {
                    enemyUnitIndex = internalEnemyUnitIndex;
                    teamsDirty = true;
                    MOVE_CURSOR_UP((numberOfTeams * 2) + 4);
                }

                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
            }

            if (_menuSelectedIndex == 3 && lockedItems[3] == false) {
                printf("Attack enemy (%llu) from team (%llu)\n", enemyTeamIndex, enemyUnitIndex);
                Sleep(1000);

                attackUnit(teams[currentTeamIndex], currentUnitIndex, teams[enemyTeamIndex], enemyUnitIndex);

                if (Team_isDefeat(teams[currentTeamIndex])) {
                    currentLastTeamIndex = enemyTeamIndex;
                    teamsAlive--;
                }
                else if (Team_isDefeat(teams[enemyTeamIndex])) {
                    currentLastTeamIndex = currentTeamIndex;
                    teamsAlive--;
                }

                currentUnitIndex = -1;
                enemyTeamIndex = -1;
                enemyUnitIndex = -1;
                _menuSelectedIndex = 0;

                currentTeamIndex = (currentTeamIndex + 1) % numberOfTeams;

                MOVE_CURSOR_UP((numberOfTeams * 2) + 4);
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
                teamsDirty = true;
            }

            if (_menuSelectedIndex == 4) {
                printf("Skip round\n");
                currentTeamIndex = (currentTeamIndex + 1) % numberOfTeams;

                currentUnitIndex = -1;
                enemyTeamIndex = -1;
                enemyUnitIndex = -1;
                _menuSelectedIndex = 0;

                MOVE_CURSOR_UP((numberOfTeams * 2) + 4);
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
                teamsDirty = true;
            }

            if (_menuSelectedIndex == 5) {
                printf("Exit from game\n");

                MOVE_CURSOR_UP((numberOfTeams * 2) + 4);
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
                teamsDirty = true;

                return;
            }
        }
    }

    printf("Team \"%s\" is win!\n", teams[currentLastTeamIndex]->name);
    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

    Sleep(5000);
    MOVE_CURSOR_UP(1);
    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

    for (size_t index = 0; index < numberOfTeams; index++) {
        Team_deconstructor(teams[index]);
    }

    free(teams);
}

void pveSingleplayerGameLoop() {
    Team** teams;
    size_t numberOfTeams;
    size_t numberOfUnitsInTeams;

    #define MAX_RESULT_LENGTH 10
    char result[MAX_RESULT_LENGTH];

    while (true) {
        printf("Enter the number of teams (by default %llu): ", DEFAULT_NUMBER_OF_TEAMS);
        printfWithRGBColor(&DEFEAT_TEAM_UNIT_COLOR, NULL, "2");
        MOVE_CURSOR_LEFT(1);

        scanString(result, MAX_RESULT_LENGTH);

        if (result[0] == '\0') {
            numberOfTeams = DEFAULT_NUMBER_OF_TEAMS;
            break;
        }
        else if (isCharNumber(result) && !isNegativeCharNumber(result)) {
            numberOfTeams = charNumberToDecimal(result);

            if (numberOfTeams > 1) {
                break;
            }
        }
        else {
            MOVE_CURSOR_UP_HOME(1);
            ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
            printf("Invalid value\n");
            Sleep(2000);
            MOVE_CURSOR_UP_HOME(1);
            ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

            numberOfTeams = 0;
        }
    }

    while (true) {
        printf("Enter the number of units in teams (by default %llu): ", DEFAULT_NUMBER_OF_UNITS_IN_TEAMS);
        printfWithRGBColor(&DEFEAT_TEAM_UNIT_COLOR, NULL, "1");
        MOVE_CURSOR_LEFT(1);

        scanString(result, MAX_RESULT_LENGTH);

        if (result[0] == '\0') {
            numberOfUnitsInTeams = DEFAULT_NUMBER_OF_UNITS_IN_TEAMS;
            break;
        }
        else if (isCharNumber(result) && !isNegativeCharNumber(result)) {
            numberOfUnitsInTeams = charNumberToDecimal(result);

            if (numberOfUnitsInTeams > 0) {
                break;
            }
        }
        else {
            MOVE_CURSOR_UP_HOME(1);
            ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
            printf("Invalid value\n");
            Sleep(2000);
            MOVE_CURSOR_UP_HOME(1);
            ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

            numberOfUnitsInTeams = 0;
        }
    }

    MOVE_CURSOR_UP(2);
    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;



    teams = calloc(numberOfTeams, sizeof(Team*));

    teams[0] = Team_constructor(CURRENT_USER.login == NULL ? "user" : CURRENT_USER.login, numberOfUnitsInTeams);

    for (size_t index = 1; index < numberOfTeams; index++) {
        char name[MAX_TEAM_NAME_LENGTH];
        sprintf(name, "bot #%zu", index);

        teams[index] = Team_constructor(name, numberOfUnitsInTeams);
    }

    Sleep(1000);



    size_t currentTeamIndex = 0;
    size_t currentLastTeamIndex = 0;
    size_t teamsAlive = numberOfTeams;

    size_t currentUnitIndex = -1;
    size_t enemyTeamIndex = -1;
    size_t enemyUnitIndex = -1;

#define ITEMS_SIZE 6
    char* items[ITEMS_SIZE] = {
            "Select unit for attack",
            "Select team to attack",
            "Select unit to attack",
            "Attack enemy",
            "Skip round",
            "Stop game"
    };

    size_t selectedIndex = 0;

    bool teamsDirty = true;

    while(teamsAlive > 1) {
        while (Team_isDefeat(teams[currentTeamIndex])) {
            currentTeamIndex = (currentTeamIndex + 1) % numberOfTeams;
        }

        if (teamsDirty == true) {
            ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
            teamsDirty = false;
            printf("Round for team \"%s\" (#%llu)\n", teams[currentTeamIndex]->name, currentTeamIndex + 1);

            printTeams(
                    teams,
                    numberOfTeams,
                    numberOfUnitsInTeams,
                    currentTeamIndex,
                    currentUnitIndex,
                    enemyTeamIndex,
                    enemyUnitIndex
            );
        }

        //bot behavior
        if (currentTeamIndex > 0) {
            if (currentUnitIndex == -1) {
                printf("Bot selecting current unit...\n");
                Sleep(10 * BOT_BRAIN_SPEED_MULTIPLIER);
                MOVE_CURSOR_UP(1);
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                while (currentUnitIndex == -1 || teams[currentTeamIndex]->units[currentUnitIndex]->power == 0) {
                    currentUnitIndex = rand() % numberOfUnitsInTeams;
                }

                printf("Bot selected current unit\n");
                Sleep(DELAY_AFTER_BOT_SELECT);
                MOVE_CURSOR_UP(1);
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                IF_BOT_DELAY {
                    teamsDirty = true;
                    MOVE_CURSOR_UP((numberOfTeams * 2) + 3);
                    continue;
                };
            }

            if (enemyTeamIndex == -1) {
                printf("Bot selecting enemy team...\n");
                Sleep(10 * BOT_BRAIN_SPEED_MULTIPLIER);
                MOVE_CURSOR_UP(1);
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                while (enemyTeamIndex == -1 || enemyTeamIndex == currentTeamIndex || Team_isDefeat(teams[enemyTeamIndex])) {
                    enemyTeamIndex = rand() % numberOfTeams;
                }

                printf("Bot selected current enemy team\n");
                Sleep(DELAY_AFTER_BOT_SELECT);
                MOVE_CURSOR_UP(1);
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                IF_BOT_DELAY {
                    teamsDirty = true;
                    MOVE_CURSOR_UP((numberOfTeams * 2) + 3);
                    continue;
                };
            }

            if (enemyUnitIndex == -1) {
                printf("Bot selecting enemy unit...\n");
                Sleep(10 * BOT_BRAIN_SPEED_MULTIPLIER);
                MOVE_CURSOR_UP(1);
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                while (enemyUnitIndex == -1 || teams[enemyTeamIndex]->units[enemyUnitIndex]->power == 0) {
                    enemyUnitIndex = rand() % numberOfUnitsInTeams;
                }

                printf("Bot selected current enemy unit\n");
                Sleep(DELAY_AFTER_BOT_SELECT);
                MOVE_CURSOR_UP(1);
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                IF_BOT_DELAY {
                    teamsDirty = true;
                    MOVE_CURSOR_UP((numberOfTeams * 2) + 3);
                    continue;
                };
            }

            printf("Bot attacking enemy...\n");
            Sleep(DELAY_AFTER_BOT_SELECT * 2);
            MOVE_CURSOR_UP(1);
            ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

            attackUnit(teams[currentTeamIndex], currentUnitIndex, teams[enemyTeamIndex], enemyUnitIndex);

            if (Team_isDefeat(teams[currentTeamIndex])) {
                currentLastTeamIndex = enemyTeamIndex;
                teamsAlive--;
            }
            else if (Team_isDefeat(teams[enemyTeamIndex])) {
                currentLastTeamIndex = currentTeamIndex;
                teamsAlive--;
            }

            currentUnitIndex = -1;
            enemyTeamIndex = -1;
            enemyUnitIndex = -1;

            teamsDirty = true;
            MOVE_CURSOR_UP((numberOfTeams * 2) + 3);
            currentTeamIndex = (currentTeamIndex + 1) % numberOfTeams;
            continue;
        }

        bool lockedItems[ITEMS_SIZE] = {
            false,
            false,
            enemyTeamIndex == -1,
            currentUnitIndex == -1 || enemyTeamIndex == -1 || enemyUnitIndex == -1,
            false,
            false
        };

        SelectMenuOptions options = {
                ITEMS_SIZE,
                items,
                lockedItems,
                SELECTED_COLOR,
                SELECTED_BACKGROUND_COLOR,
                LOCKED_COLOR,
                LOCKED_BACKGROUND_COLOR,
                SELECTED_LOCKED_COLOR,
                SELECTED_LOCKED_BACKGROUND_COLOR
        };

        printSelectMenu2(options, selectedIndex);

        MOVE_CURSOR_UP(ITEMS_SIZE);
        char input = getch();

        if (input == 'w' || input == 72) {
            selectedIndex = selectedIndex == 0 ? ITEMS_SIZE - 1 : selectedIndex - 1;
        }
        else if (input == 's' || input == 80) {
            selectedIndex = selectedIndex + 1 >= ITEMS_SIZE ? 0 : selectedIndex + 1;
        }
        else if (input == '\n' || input == 13 || input == 100) {
            ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

            if (selectedIndex == 0) {
                size_t internalCurrentUnitIndex = currentUnitIndex;

                printf("Enter index (0 <= index < %llu) of unit for attack: ", numberOfUnitsInTeams);
                scanf("%llu", &internalCurrentUnitIndex);

                if (internalCurrentUnitIndex >= numberOfUnitsInTeams) {
                    MOVE_CURSOR_UP(1);
                    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                    printf("Invalid unit index\n");
                    Sleep(2000);
                    MOVE_CURSOR_UP(1);
                }
                else if (teams[currentTeamIndex]->units[internalCurrentUnitIndex]->power == 0) {
                    MOVE_CURSOR_UP(1);
                    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                    printf("Invalid unit (%llu) because is have zero power\n", internalCurrentUnitIndex);
                    Sleep(2000);
                    MOVE_CURSOR_UP(1);
                }
                else {
                    currentUnitIndex = internalCurrentUnitIndex;
                    teamsDirty = true;
                    MOVE_CURSOR_UP((numberOfTeams * 2) + 4);
                }

                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
            }

            if (selectedIndex == 1) {
                size_t internalEnemyTeamIndex = enemyTeamIndex;

                printf("Enter index (0 <= index < %llu) of enemy team: ", numberOfTeams);
                scanf("%llu", &internalEnemyTeamIndex);

                if (internalEnemyTeamIndex >= numberOfTeams) {
                    MOVE_CURSOR_UP(1);
                    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                    printf("Invalid enemy team index\n");
                    Sleep(2000);
                    MOVE_CURSOR_UP(1);
                }
                else if (currentTeamIndex == internalEnemyTeamIndex) {
                    MOVE_CURSOR_UP(1);
                    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                    printf("Invalid team (%llu) because is your team man....\n", internalEnemyTeamIndex);
                    Sleep(2000);
                    MOVE_CURSOR_UP(1);
                }
                else {
                    enemyTeamIndex = internalEnemyTeamIndex;
                    enemyUnitIndex = -1;
                    teamsDirty = true;
                    MOVE_CURSOR_UP((numberOfTeams * 2) + 4);
                }

                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
            }

            if (selectedIndex == 2 && lockedItems[2] == false) {
                size_t internalEnemyUnitIndex = enemyUnitIndex;

                printf("Enter index (0 <= index < %llu) of unit to attack: ", numberOfUnitsInTeams);
                scanf("%llu", &internalEnemyUnitIndex);

                if (internalEnemyUnitIndex >= numberOfUnitsInTeams) {
                    MOVE_CURSOR_UP(1);
                    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                    printf("Invalid enemy unit index\n");
                    Sleep(2000);
                    MOVE_CURSOR_UP(1);
                }
                else if (teams[enemyTeamIndex]->units[internalEnemyUnitIndex]->power == 0) {
                    MOVE_CURSOR_UP(1);
                    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                    printf("Invalid unit (%llu) because is have zero power\n", internalEnemyUnitIndex);
                    Sleep(2000);
                    MOVE_CURSOR_UP(1);
                }
                else {
                    enemyUnitIndex = internalEnemyUnitIndex;
                    teamsDirty = true;
                    MOVE_CURSOR_UP((numberOfTeams * 2) + 4);
                }

                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
            }

            if (selectedIndex == 3 && lockedItems[3] == false) {
                printf("Attack enemy (%llu) from team (%llu)\n", enemyTeamIndex, enemyUnitIndex);
                Sleep(1000);

                attackUnit(teams[currentTeamIndex], currentUnitIndex, teams[enemyTeamIndex], enemyUnitIndex);

                if (Team_isDefeat(teams[currentTeamIndex])) {
                    currentLastTeamIndex = enemyTeamIndex;
                    teamsAlive--;
                }
                else if (Team_isDefeat(teams[enemyTeamIndex])) {
                    currentLastTeamIndex = currentTeamIndex;
                    teamsAlive--;
                }

                currentUnitIndex = -1;
                enemyTeamIndex = -1;
                enemyUnitIndex = -1;
                selectedIndex = 0;

                currentTeamIndex = (currentTeamIndex + 1) % numberOfTeams;

                MOVE_CURSOR_UP((numberOfTeams * 2) + 4);
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
                teamsDirty = true;
            }

            if (selectedIndex == 4) {
                printf("Skip round\n");
                currentTeamIndex = (currentTeamIndex + 1) % numberOfTeams;

                currentUnitIndex = -1;
                enemyTeamIndex = -1;
                enemyUnitIndex = -1;
                selectedIndex = 0;

                MOVE_CURSOR_UP((numberOfTeams * 2) + 4);
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
                teamsDirty = true;
            }

            if (selectedIndex == 5) {
                printf("Exit from game\n");

                MOVE_CURSOR_UP((numberOfTeams * 2) + 4);
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
                teamsDirty = true;

                return;
            }
        }
    }

    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
    printf("Team \"%s\" is win!\n", teams[currentLastTeamIndex]->name);

    Sleep(5000);
    MOVE_CURSOR_UP(1);
    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

    for (size_t index = 0; index < numberOfTeams; index++) {
        Team_deconstructor(teams[index]);
    }

    free(teams);
}
//#endregion

int main() {
    ERASE_DISPLAY;

    srand(time(NULL));

    printf(" ________  ________  _____ ______   _______           ________  ________      _________  ___  ___  _______           _________  _______   ________  _____ ______   ________      \n"
           "|\\   ____\\|\\   __  \\|\\   _ \\  _   \\|\\  ___ \\         |\\   __  \\|\\  _____\\    |\\___   ___\\\\  \\|\\  \\|\\  ___ \\         |\\___   ___\\\\  ___ \\ |\\   __  \\|\\   _ \\  _   \\|\\   ____\\     \n"
           "\\ \\  \\___|\\ \\  \\|\\  \\ \\  \\\\\\__\\ \\  \\ \\   __/|        \\ \\  \\|\\  \\ \\  \\__/     \\|___ \\  \\_\\ \\  \\\\\\  \\ \\   __/|        \\|___ \\  \\_\\ \\   __/|\\ \\  \\|\\  \\ \\  \\\\\\__\\ \\  \\ \\  \\___|_    \n"
           " \\ \\  \\  __\\ \\   __  \\ \\  \\\\|__| \\  \\ \\  \\_|/__       \\ \\  \\\\\\  \\ \\   __\\         \\ \\  \\ \\ \\   __  \\ \\  \\_|/__           \\ \\  \\ \\ \\  \\_|/_\\ \\   __  \\ \\  \\\\|__| \\  \\ \\_____  \\   \n"
           "  \\ \\  \\|\\  \\ \\  \\ \\  \\ \\  \\    \\ \\  \\ \\  \\_|\\ \\       \\ \\  \\\\\\  \\ \\  \\_|          \\ \\  \\ \\ \\  \\ \\  \\ \\  \\_|\\ \\           \\ \\  \\ \\ \\  \\_|\\ \\ \\  \\ \\  \\ \\  \\    \\ \\  \\|____|\\  \\  \n"
           "   \\ \\_______\\ \\__\\ \\__\\ \\__\\    \\ \\__\\ \\_______\\       \\ \\_______\\ \\__\\            \\ \\__\\ \\ \\__\\ \\__\\ \\_______\\           \\ \\__\\ \\ \\_______\\ \\__\\ \\__\\ \\__\\    \\ \\__\\____\\_\\  \\ \n"
           "    \\|_______|\\|__|\\|__|\\|__|     \\|__|\\|_______|        \\|_______|\\|__|             \\|__|  \\|__|\\|__|\\|_______|            \\|__|  \\|_______|\\|__|\\|__|\\|__|     \\|__|\\_________\\\n"
           "                                                                                                                                                                     \\|_________|\n");

    printf(
        "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n"
        "Created by nikyoff for omgups 2023 -- https://github.com/NikyOFF/omgups23/tree/assignment\n"
        "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\n\n"
    );

    int iResult;

//    do {
//        printf("Press any button to start!\n");
//        getch();
//        MOVE_CURSOR_UP(1llu);
//
//        iResult = connectToServer();
//        MOVE_CURSOR_UP(1llu);
//        ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
//    } while (iResult != 0);
//
//
//    if (iResult != 0) {
//        exit(0);
//    }
//
//    iResult = processAuth(&CURRENT_USER);
//
//    if (iResult == INVALID_SOCKET_PROCESS_AUTH_RESULT) {
//        handleSocketError();
//        return 1;
//    }
//
//    if (iResult == INVALID_PASSWORD_PROCESS_AUTH_RESULT) {
//        printf("Invalid password\n");
//        return 1;
//    }

//    printf("[main] Authorized is SUCCESS\n\n");
//
//    Sleep(1000);
//    MOVE_CURSOR_UP(4llu);
//    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

    printf("[main] Welcome \"%s\", to game! Press any button to play...\n", CURRENT_USER.login);
    getch();

    MOVE_CURSOR_UP(1llu);
    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

    while (CURRENT_SCENE != EXIT_SCENE) {
        if (CURRENT_SCENE == MAIN_MENU_SCENE) {
            mainMenuSceneIteration();
        }
        else if (CURRENT_SCENE == PVP_SINGLEPLAYER_SCENE) {
            pvpSingleplayerGameLoop();

            CURRENT_SCENE = MAIN_MENU_SCENE;
            CURRENT_MENU = MAIN_MENU_MENU;
        }
        else if (CURRENT_SCENE == PVE_SINGLEPLAYER_SCENE) {
            pveSingleplayerGameLoop();

            CURRENT_SCENE = MAIN_MENU_SCENE;
            CURRENT_MENU = MAIN_MENU_MENU;
        }
        else if (CURRENT_SCENE == SERVER_SCENE) {
            if (IS_SERVER_OWNER == true) {

            }
            else {
                int result = rpcConnectionToServer(CLIENT_SOCKET, CURRENT_GAME_SERVER.id);
            }

            printf("Current server: %s\n", CURRENT_GAME_SERVER.serverName);

            Sleep(10000000);
            CURRENT_SCENE = MAIN_MENU_SCENE;
            CURRENT_MENU = MAIN_MENU_MENU;
        }
    }


    closesocket(CLIENT_SOCKET);
    WSACleanup();

    ERASE_DISPLAY;
    return 0;
}



void example() {
    INITIALIZE_SELECT_MENU(3, 0, "option 1", "option 2", "option 3");

    while (true) {
        SELECT_MENU_BEHAVIOR() {

            if (_menuSelectedIndex == 0) {
                //option 1
            }
            else if (_menuSelectedIndex == 1) {
                //option 2
            }
            else {
                //option 3
            }

            _menuSelectedIndex = 0;
        }
    }
}
