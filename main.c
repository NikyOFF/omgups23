#include "client.h"

SOCKET CLIENT_SOCKET = INVALID_SOCKET;

#define EXIT_SCENE 0

#define MAIN_MENU_SCENE 1

#define MAIN_MENU_MENU 10
#define START_NEW_GAME_MENU 11
#define SINGLEPLAYER_MENU 110
#define MULTIPLAYER_MENU 111
#define SELECT_SERVER_MENU 1110
#define STATISTICS_MENU 12
#define LOGOUT_MENU 13


#define SERVER_SCENE 2

#define PVE_SINGLEPLAYER_SCENE 20

#define PVP_SINGLEPLAYER_SCENE 30


size_t CURRENT_SCENE = MAIN_MENU_SCENE;
size_t CURRENT_MENU = MAIN_MENU_MENU;
User CURRENT_USER;

bool IS_SERVER_OWNER = false;
GameServer CURRENT_GAME_SERVER;


void handleSocketError() {

}

typedef struct RGBStruct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} RGB;

void printfWithRGBColor(
        RGB backgroundColor,
        RGB foregroundColor,
        const char* format,
        ...
) {
    HANDLE consoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    printf(
            "\x1b[48;2;%u;%u;%um\x1b[38;2;%u;%u;%um",
            backgroundColor.red,
            backgroundColor.green,
            backgroundColor.blue,
            foregroundColor.red,
            foregroundColor.green,
            foregroundColor.blue
    );
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    SetConsoleTextAttribute(consoleOutput, 7);
}

void printfWithColor(const WORD wAttributes, const char* format, ...) {
    HANDLE consoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(consoleOutput, wAttributes);
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    SetConsoleTextAttribute(consoleOutput, 7);
}


void printSelectMenu(char** items, size_t itemsSize, size_t selectedIndex) {

    for (size_t index = 0; index < itemsSize; index++) {
        char* item = items[index];

        printf("%zu.", index+1);

        if (index == selectedIndex) {
            printfWithColor(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | 0x80, " %s ", item);
        }
        else {
            printf(" %s ", item);
        }

        printf("\n");
    }
}

typedef struct SelectMenuOptionsStruct {
    size_t itemsSize;
    char** items;
    bool* lockedItems;

    RGB selectedColor;
    RGB selectedBackgroundColor;

    RGB lockedColor;
    RGB lockedBackgroundColor;

    RGB selectedLockedColor;
    RGB selectedLockedBackgroundColor;
} SelectMenuOptions;

void printSelectMenu2(SelectMenuOptions options, size_t selectedIndex) {
    for (size_t index = 0; index < options.itemsSize; index++) {
        char* item = options.items[index];

        printf("%zu.", index+1);

        if (index == selectedIndex) {

            if (options.lockedItems[index] == true) {
                printfWithRGBColor(
                        options.selectedLockedBackgroundColor,
                        options.selectedLockedColor,
                        " %s ",
                        item
                );
            }
            else {
                printfWithRGBColor(
                        options.selectedBackgroundColor,
                        options.selectedColor,
                        " %s ",
                        item
                );
            }
        }
        else if (options.lockedItems[index] == true) {
            printfWithRGBColor(
                    options.lockedBackgroundColor,
                    options.lockedColor,
                    " %s ",
                    item
            );
        }
        else {
            printf(" %s ", item);
        }

        printf("\n");
    }

}

const RGB WHITE_COLOR = {255, 255, 255};

const RGB SELECTED_COLOR = {0, 0, 0};
const RGB SELECTED_BACKGROUND_COLOR = {200, 200, 200};

const RGB LOCKED_COLOR = {0, 0, 0};
const RGB LOCKED_BACKGROUND_COLOR = {100, 100, 100};

const RGB SELECTED_LOCKED_COLOR = {200, 200, 200};
const RGB SELECTED_LOCKED_BACKGROUND_COLOR = {120, 120, 120};


const RGB TEAM_UNIT_COLOR = {0, 180, 0};
const RGB SELECTED_TEAM_UNIT_COLOR = {0, 255, 0};

const RGB ENEMY_TEAM_UNIT_COLOR = {180, 0, 0};
const RGB SELECTED_ENEMY_TEAM_UNIT_COLOR = {255, 0, 0};



int connectToServer() {
    int iResult;
    WSADATA wsData;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsData);

    if (iResult != 0) {
        printf("[main] Failed to initialize socket\n");
        return 1;
    }

    CLIENT_SOCKET = socket(AF_INET, SOCK_STREAM, 0);

    if (CLIENT_SOCKET == INVALID_SOCKET) {
        printf("[main] Failed to create socket\n");
        return 1;
    }

    printf("[main] Server socket initialization is OK\n");

    struct in_addr ip_to_num;

    iResult = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);

    if (iResult <= 0) {
        printf("[main] Error in IP translation to special numeric format\n");
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
        return 1;
    }

    printf("[main] Connection established SUCCESSFULLY. Ready to send a message to Server\n\n");
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

void logoutMenuIteration() {}

void mainMenuIteration() {
    char* items[4] = {"New game", "Statistic", "Logout", "Exit"};
    size_t itemsSize = 4;
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
                    CURRENT_MENU = LOGOUT_MENU;
                }

                if (selectedIndex == 3) {
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
        case LOGOUT_MENU:
            logoutMenuIteration();
            break;
    }
}


typedef struct UnitStruct {
    size_t power;
} Unit;

Unit* Unit_constructor(size_t power) {
    Unit* unit = calloc(1, sizeof(Unit));

    unit->power = power;

    return unit;
}

void Unit_deconstructor(Unit* unit) {
    free(unit);
}

int attackUnit(Unit* unit, Unit* target) {
    if (unit->power == 0) {
        return 1;
    }

    if (target->power == 0) {
        return 2;
    }

    size_t attackPower = (rand() % unit->power) + 1;
    size_t protectionPower = (rand() % target->power) + 1;

    if (attackPower < protectionPower) {
        unit->power -= attackPower;
        target->power += attackPower;
    }
    else if (protectionPower < attackPower) {
        target->power -= protectionPower;
        unit->power += protectionPower;
    }
    else {
        size_t side = rand() % 2;

        if (side == 0) {
            unit->power -= attackPower;
            target->power += attackPower;
        }
        else {
            target->power -= protectionPower;
            unit->power += protectionPower;
        }
    }

    return 0;
}

typedef struct TeamStruct {
    char* name;
    size_t unitsLength;
    Unit** units;
} Team;

Team* Team_constructor(char* name, size_t unitsLength) {
    Team* team = calloc(2, sizeof(Team));

    char* nameCopy = calloc(strlen(name), sizeof(char));
    strcpy(nameCopy, name);

    team->name = nameCopy;
    team->unitsLength = unitsLength;
    team->units = calloc(unitsLength, sizeof(Unit*));

    for (size_t index = 0; index < unitsLength; index++) {
        team->units[index] = Unit_constructor(1);
    }

    return team;
}

void Team_deconstructor(Team* team) {
    for (size_t index = 0; index < team->unitsLength; index++) {
        free(team->units[index]);
    }

    free(team->name);
    free(team->units);
    free(team);
}


#define MAX_TEAM_NAME_LENGTH 50

void printTeams(
        Team** teams,
        size_t numberOfTeams,
        size_t numberOfUnitsInTeams,
        size_t currentTeamIndex,
        size_t selectedUnit,
        size_t enemyTeamIndex,
        size_t unitToAttack
) {

    for (size_t teamIndex = 0; teamIndex < numberOfTeams; teamIndex++) {
        Team* team = teams[teamIndex];

        for (size_t unitIndex = 0; unitIndex < numberOfUnitsInTeams; unitIndex++) {
            Unit* unit = team->units[unitIndex];

            if (currentTeamIndex != -1 && currentTeamIndex == teamIndex) {
                printfWithRGBColor(
                    selectedUnit != -1 && selectedUnit == unitIndex ? SELECTED_TEAM_UNIT_COLOR : TEAM_UNIT_COLOR,
                    WHITE_COLOR,
                    " [%llu] ",
                    unit->power
                );

                continue;
            }

            if (enemyTeamIndex != -1 && enemyTeamIndex == teamIndex) {
                printfWithRGBColor(
                    unitToAttack != -1 && unitToAttack == unitIndex ? SELECTED_ENEMY_TEAM_UNIT_COLOR : ENEMY_TEAM_UNIT_COLOR,
                    WHITE_COLOR,
                    " [%llu] ",
                    unit->power
                );

                continue;
            }

            printf(" [%llu] ", unit->power);

        }

        if (currentTeamIndex != -1 && currentTeamIndex == teamIndex) {
            printfWithColor(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | 0x80, "   (%llu) Team \"%s\" \n\n", teamIndex, team->name);
        }
        else {
            printf("   (%llu) Team \"%s\" \n\n", teamIndex, team->name);
        }
    }

    printf("----------------------------------------------------------------------------------------------------------------------------\n\n");

}

void pvpSingleplayerGameLoop() {
    Team** teams;
    size_t numberOfTeams;
    size_t numberOfUnitsInTeams;

    printf("Enter the number of teams: ");
    scanf("%llu", &numberOfTeams);

    printf("Enter the number of units in teams: ");
    scanf("%llu", &numberOfUnitsInTeams);

    teams = calloc(numberOfTeams, sizeof(Team*));

    for (size_t index = 0; index < numberOfTeams; index++) {
        char name[MAX_TEAM_NAME_LENGTH];
        printf("Enter the name for team #%llu: ", index + 1);
        scanf("%s", name);

        teams[index] = Team_constructor(name, numberOfUnitsInTeams);
    }

    Sleep(1000);
    MOVE_CURSOR_UP(numberOfTeams + 2);
    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

    size_t currentTeamIndex = 0;

    while (true) {
        size_t currentUnitIndex = -1;
        size_t enemyTeamIndex = -1;
        size_t enemyUnitIndex = -1;

        char* items[6] = {
                "Select unit for attack",
                "Select team to attack",
                "Select unit to attack",
                "Attack enemy",
                "Skip round",
                "Stop game"
        };

        size_t itemsSize = 6;
        size_t selectedIndex = 0;

        bool teamsDirty = true;

        while(1) {
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
                    itemsSize,
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

            MOVE_CURSOR_UP(itemsSize);
            char input = getch();

            if (input == 'w' || input == 72) {
                selectedIndex = selectedIndex == 0 ? itemsSize - 1 : selectedIndex - 1;
            }
            else if (input == 's' || input == 80) {
                selectedIndex = selectedIndex + 1 >= itemsSize ? 0 : selectedIndex + 1;
            }
            else if (input == '\n' || input == 13 || input == 100) {
                ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

                if (selectedIndex == 0) {
                    printf("Enter index (0 <= index < %llu) of unit for attack: ", numberOfUnitsInTeams);
                    scanf("%llu", &currentUnitIndex);

                    MOVE_CURSOR_UP((numberOfTeams * 2) + 4);
                    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
                    teamsDirty = true;
                }

                if (selectedIndex == 1) {
                    printf("Enter index (0 <= index < %llu) of enemy team: ", numberOfTeams);
                    scanf("%llu", &enemyTeamIndex);

                    MOVE_CURSOR_UP((numberOfTeams * 2) + 4);
                    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
                    teamsDirty = true;
                }

                if (selectedIndex == 2 && lockedItems[2] == false) {
                    printf("Enter index (0 <= index < %llu) of unit to attack: ", numberOfUnitsInTeams);
                    scanf("%llu", &enemyUnitIndex);

                    MOVE_CURSOR_UP((numberOfTeams * 2) + 4);
                    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
                    teamsDirty = true;
                }

                if (selectedIndex == 3 && lockedItems[3] == false) {
                    printf("Attack team (%llu) enemy (%llu)\n", enemyTeamIndex, enemyUnitIndex);
                    Sleep(1000);

                    attackUnit(teams[currentTeamIndex]->units[currentUnitIndex], teams[enemyTeamIndex]->units[enemyUnitIndex]);

                    currentUnitIndex = -1;
                    enemyTeamIndex = -1;
                    enemyUnitIndex = -1;
                    selectedIndex = 0;

                    currentTeamIndex++;
                    currentTeamIndex = currentTeamIndex >= numberOfTeams ? 0 : currentTeamIndex;

                    MOVE_CURSOR_UP((numberOfTeams * 2) + 4);
                    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;
                    teamsDirty = true;
                }

                if (selectedIndex == 4) {
                    printf("Skip round\n");
                    currentTeamIndex++;
                    currentTeamIndex = currentTeamIndex >= numberOfTeams ? 0 : currentTeamIndex;

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
    }

    Sleep(5000);

    for (size_t index = 0; index < numberOfTeams; index++) {
        Team_deconstructor(teams[index]);
    }

    free(teams);
}

void pveSingleplayerGameLoop() {

}

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

    printf("Press any button to start!\n");
    getch();
    MOVE_CURSOR_UP(1llu);

    int iResult;
    iResult = connectToServer();

    if (iResult != 0) {
        exit(0);
    }

    iResult = processAuth(&CURRENT_USER);

    if (iResult == INVALID_SOCKET_PROCESS_AUTH_RESULT) {
        handleSocketError();
        return 1;
    }

    if (iResult == INVALID_PASSWORD_PROCESS_AUTH_RESULT) {
        printf("Invalid password\n");
        return 1;
    }

    printf("[main] Authorized is SUCCESS\n\n");

    Sleep(1000);
    MOVE_CURSOR_UP(7llu);
    ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN;

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
            printf("PVE singleplayer");

            Sleep(10000);
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
