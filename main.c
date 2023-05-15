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
GameServer CURRENT_GAME_SERVER;


void handleSocketError() {

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

int receiveServerPacket(SOCKET socket, Binary* serverPacket, int flags) {
    size_t defaultReadIndex = serverPacket->readIndex;

    int recvResult;
    bool flag = true;

    while(flag == true) {
        Binary_clear(&serverPacket);
        recvResult = recv(CLIENT_SOCKET, serverPacket->buffer, serverPacket->bufferSize, flags);

        if (recvResult == -1) {
            return -1;
        }

        if (recvResult == SERVER_SOCKET_PING_PACKET_SIZE) {
            size_t strSize;
            char* str = Binary_readString(&serverPacket, &strSize);

            if (strSize == SERVER_SOCKET_PING_PACKET_MESSAGE_SIZE && strcmp(str, SERVER_SOCKET_PING_PACKET_MESSAGE) == 0) {
                Binary clientPacket = *Binary_constructor(CLIENT_SOCKET_PING_PACKET_SIZE);
                Binary_writeString(&clientPacket, CLIENT_SOCKET_PING_PACKET_MESSAGE);

                int iResult = send(CLIENT_SOCKET, clientPacket.buffer, CLIENT_SOCKET_PING_PACKET_SIZE, 0);

                Sleep(100);
                free(clientPacket.buffer);

                if (iResult == -1) {
                    return -1;
                }

                continue;
            }
        }

        flag = false;
    }

    serverPacket->readIndex = defaultReadIndex;
    return recvResult;
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
                selectedIndex = selectedIndex == 0 ? 0 : selectedIndex - 1;
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
                selectedIndex = selectedIndex == 0 ? 0 : selectedIndex - 1;
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
                selectedIndex = selectedIndex == 0 ? 0 : selectedIndex - 1;
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
                selectedIndex = selectedIndex == 0 ? 0 : selectedIndex - 1;
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
                selectedIndex = selectedIndex == 0 ? 0 : selectedIndex - 1;
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

int main() {
    ERASE_DISPLAY;

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
            printf("PVP singleplayer");
            Sleep(10000);
        }
        else if (CURRENT_SCENE == PVE_SINGLEPLAYER_SCENE) {
            printf("PVE singleplayer");
            Sleep(10000);
        }
        else if (CURRENT_SCENE == SERVER_SCENE) {
            printf("Current server: %s\n", CURRENT_GAME_SERVER.serverName);
            Sleep(10000);
        }
    }


    closesocket(CLIENT_SOCKET);
    WSACleanup();

    return 0;
}
