#include "server.h"

#pragma comment(lib, "Ws2_32.lib")

SOCKET SERVER_SOCKET = INVALID_SOCKET;
pthread_mutex_t GENERAL_AUTH_MUTEX;

pthread_mutex_t CONNECTIONS_MUTEX;
Connection_vec_t CONNECTIONS_VEC;

size_t LAST_GAME_SERVER_ID = 0;
pthread_mutex_t GAME_SERVERS_MUTEX;
GameServer_vec_t GAME_SERVER_VEC;

Connection* getConnectionBySocket(SOCKET* socket) {
    int index;
    Connection* connection;

    vec_foreach(&CONNECTIONS_VEC, connection, index) {
        if (&connection->socket == socket) {
            return connection;
        }
    }

    return NULL;
}

GameServer* getGameServerById(size_t id) {
    int index;
    GameServer* gameServer;

    vec_foreach(&GAME_SERVER_VEC, gameServer, index) {
        if (gameServer->id == id) {
            return gameServer;
        }
    }

    return NULL;
}

void connectionLoop(Connection* connection) {
    int iResult;
    Binary* clientPacket = Binary_constructor(512);

    while (connection->socket != INVALID_SOCKET) {
        Binary_clear(clientPacket);

        printfForConnection(connection, "Wait rpc command from client socket");
        iResult = recv(connection->socket, clientPacket->buffer, 512, 0);

        if (iResult == -1) {
            break;
        }

        size_t rpcCommandSize;
        char* rpcCommand = Binary_readString(clientPacket, &rpcCommandSize);


        printfForConnection(connection, "Handle rpc command from client socket: %s\n", rpcCommand);

        if (strcmp(rpcCommand, "rpc:create_game_server") == 0) {
            size_t serverNameSize;
            char* serverName = Binary_readString(clientPacket, &serverNameSize);

            GameServer* gameServer = GameServer_constructor(LAST_GAME_SERVER_ID, serverName, connection->socket);
            LAST_GAME_SERVER_ID++;

            pthread_mutex_lock(&GAME_SERVERS_MUTEX);
            vec_push(&GAME_SERVER_VEC, gameServer);
            pthread_mutex_unlock(&GAME_SERVERS_MUTEX);

            Binary* serverPacket = Binary_constructor(512);

            Binary_writeInt(serverPacket, 0);
            Binary_writeGameServer(serverPacket, gameServer);

            printf("[client %u] Create server: %s (%zu) %zub\n", connection->socket, gameServer->serverName, gameServer->id, clientPacket->writeIndex);

            if (send(connection->socket, serverPacket->buffer, serverPacket->writeIndex, 0) == -1) {
                break;
            }

            //wait connection to server
            pthread_mutex_lock(&gameServer->_serverMutex);
            pthread_cond_wait(&gameServer->_connectionRequest, &gameServer->_serverMutex);
            pthread_mutex_unlock(&gameServer->_serverMutex);

            Binary_clear(serverPacket);
            Binary_writeInt(serverPacket, 0);

            if (send(connection->socket, clientPacket->buffer, clientPacket->writeIndex, 0) == -1) {
                break;
            }

            pthread_mutex_lock(&GAME_SERVERS_MUTEX);
            vec_remove(&GAME_SERVER_VEC, gameServer);
            pthread_mutex_unlock(&GAME_SERVERS_MUTEX);

            Binary_deconstructor(serverPacket);
            //get another connection
        }
        else if (strcmp(rpcCommand, "rpc:connect_to_server") == 0) {
            size_t gameServerId;
            Binary_readSizeT(clientPacket, &gameServerId);

            GameServer* gameServer = getGameServerById(gameServerId);

            iResult = gameServer == NULL;

            Binary_clear(clientPacket);
            Binary_writeInt(clientPacket, iResult);

            if (send(connection->socket, clientPacket->buffer, clientPacket->writeIndex, 0) == -1) {
                break;
            }

            if (iResult != 0) {
                printf("[client %u] Cannot connect to server with id: %zu\n", connection->socket, gameServerId);
                continue;
            }

            pthread_mutex_lock(&gameServer->_serverMutex);
            pthread_cond_signal(&gameServer->_connectionRequest);
            pthread_mutex_unlock(&gameServer->_serverMutex);

            printf("[client %u] Connect to server: %s (%zu) [client %u]\n", connection->socket, gameServer->serverName, gameServer->id, gameServer->_owner);


            Binary_clear(clientPacket);
            Binary_writeUser(clientPacket, connection->user);

            if (send(gameServer->_owner, clientPacket->buffer, clientPacket->writeIndex, 0) == -1) {
                break;
            }
        }
        else if (strcmp(rpcCommand, "rpc:get_game_servers") == 0) {
            Binary_clear(clientPacket);
            Sleep(1000);

            Binary_writeSizeT(clientPacket, GAME_SERVER_VEC.length);

            int index;
            GameServer* gameServer;

            vec_foreach(&GAME_SERVER_VEC, gameServer, index) {
                Binary_writeGameServer(clientPacket, gameServer);
            }

            if (send(connection->socket, clientPacket->buffer, clientPacket->writeIndex, 0) == -1) {
                break;
            }
        }
    }

    Binary_deconstructor(clientPacket);
}


void printfForConnection(const Connection* connection, const char* format, ...) {
    HANDLE consoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    va_list args;
    fprintf(stdout, "[Client %llu THREAD]: ", connection->socket);
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
}


void onDisconnect(Connection* connection) {
    printfForConnection(connection, "Close connection...");

    pthread_mutex_lock(&CONNECTIONS_MUTEX);
    vec_remove(&CONNECTIONS_VEC, connection);
    pthread_mutex_unlock(&CONNECTIONS_MUTEX);

    pthread_mutex_lock(&GAME_SERVERS_MUTEX);
    int index;
    GameServer* gameServer;
    vec_foreach(&GAME_SERVER_VEC, gameServer, index) {
            if (gameServer->_owner != connection->socket) {
                continue;
            }

            vec_remove(&GAME_SERVER_VEC, gameServer);
        }
    pthread_mutex_unlock(&GAME_SERVERS_MUTEX);

    closesocket(connection->socket);
}

void* handleSocketError(Connection* connection) {
    onDisconnect(connection);
    printfForConnection(connection, "User disconnected!");

    pthread_exit(0);
    return NULL;
}


void* onConnection2(void* arg) {
    Connection* connection = (Connection *) arg;
    int iResult;

    ClientAuthPacket authPacket;
    iResult = receiveAuthPacket(connection, &authPacket);

    if (iResult == SOCKET_ERROR) {
        return handleSocketError(connection);
    }

    printfForConnection(connection, "Receive authPacket for login: %s", authPacket.login);

    User currentUser;
    bool shouldSaveUser = false;
    bool isPasswordValid = true;

    iResult = loadUserFromFile(authPacket.login, &currentUser, &GENERAL_AUTH_MUTEX);

    if (iResult == USER_FILE_NOT_FOUND) {
        shouldSaveUser = true;
        currentUser = *User_constructor(authPacket.loginSize, authPacket.login, authPacket.passwordSize, authPacket.password, 0, 0);

        iResult = sendEndAuthPacket(connection, true, &currentUser);
    }
    else {
        isPasswordValid = strcmp(authPacket.password, currentUser.password) == 0 ? true : false;
        iResult = sendEndAuthPacket(connection, isPasswordValid, &currentUser);
    }

    if (iResult == SOCKET_ERROR || isPasswordValid == false) {
        return handleSocketError(connection);
    }

    if (shouldSaveUser == true) {
        Sleep(100);
        saveUserToFile(&currentUser, &GENERAL_AUTH_MUTEX);
    }

    pthread_mutex_lock(&CONNECTIONS_MUTEX);
    connection->user = &currentUser;
    connection->connectionState = AUTHORIZED_CONNECTION_STATE;
    pthread_mutex_unlock(&CONNECTIONS_MUTEX);

    printfForConnection(connection, "User \"%s\" connected!", connection->user->login);

    connectionLoop(connection);

    onDisconnect(connection);

    printfForConnection(connection, "User \"%s\" disconnected!", connection->user->login);
    pthread_exit(0);
}

void* connectionsHealthCheckThread(void *arg) {
    int timestamp = time(NULL);

    while (1) {
        if (time(NULL) < timestamp + CONNECTIONS_CHECK_DELAY_IN_SECONDS) {
            continue;
        }

        timestamp = time(NULL);

        printf("[Connections health check THREAD] start\n");

        int index;
        Connection* connection;

        vec_foreach(&CONNECTIONS_VEC, connection, index) {
            printf("[Connections health check THREAD] check socket %lli\n", connection->socket);
        }

        printf("[Connections health check THREAD] end\n");
    }

}

void* consoleInputThread(void *arg) {
    char input[256];

    while (1) {
        if (scanf("%256s", &input) != 1) {
            continue;
        }

        if (strcmp(input, "stop") == 0 || strcmp(input, "exit") == 0) {
            exit(0);
        }

        if (strcmp(input, "status") == 0) {
            printf("Server socket status: %s\n", SERVER_SOCKET == INVALID_SOCKET ? "ERROR" : "OK");
            continue;
        }

        system(input);
    }
}

int initializeServer() {
    int iResult;

    pthread_mutex_init(&GENERAL_AUTH_MUTEX, NULL);

    pthread_mutex_init(&CONNECTIONS_MUTEX, NULL);
    vec_init(&CONNECTIONS_VEC);

    pthread_mutex_init(&GAME_SERVERS_MUTEX, NULL);
    vec_init(&GAME_SERVER_VEC);

    iResult = pthread_create(NULL, NULL, connectionsHealthCheckThread, NULL);

    if (iResult != 0) {
        printf("[main]: cannot create connections health check thread\n");
    }

    iResult = pthread_create(NULL, NULL, consoleInputThread, NULL);

    if (iResult != 0) {
        printf("[main]: cannot create console input thread\n");
    }

    CreateDirectory("players", NULL);
    Sleep(100);

    struct UserStruct user = {6, "admin", 6, "admin", 100, 0};
    saveUserToFile(&user, NULL);

    struct UserStruct *readedUser = calloc(1, sizeof(struct UserStruct));

    iResult = loadUserFromFile(user.login, readedUser, NULL);

    if (iResult != 0) {
        printf("[server] Error while trying initialize admin user\n");
    }

    printf("[server] Admin user initialized: %s\n", readedUser->login);
    User_deconstructor(readedUser);

    return 0;
}

int serverLoop() {
    int iResult;

    while(1) {

        struct sockaddr_in clientInfo;

        ZeroMemory(&clientInfo, sizeof(clientInfo));

        int clientInfo_size = sizeof(clientInfo);

        printf("[server] Listening new connection...\n");
        SOCKET clientSocket = accept(SERVER_SOCKET, (struct sockaddr*)&clientInfo, &clientInfo_size);

        if (clientSocket == INVALID_SOCKET) {
            printf("[server] Client detected, but can't connect to a client\n");
            closesocket(clientSocket);
        }

        printf("[main] Connection to a client established successfully\n");

//        Connection clientConnection = *Connection_constructor(clientSocket, AUTHORIZATION_CONNECTION_STATE, NULL);
        vec_push(&CONNECTIONS_VEC, Connection_constructor(clientSocket, AUTHORIZATION_CONNECTION_STATE, NULL));

        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);

        iResult = pthread_create(&tid, &attr, onConnection2, vec_last(&CONNECTIONS_VEC));

        if (iResult != 0) {
            printf("[server] Client connected, but can`t create new thread\n");
        }
    }
}



int main(int argc , char *argv[]) {
    int iResult;
    WSADATA wsData;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsData);

    if (iResult != 0) {
        printf("[server] Failed to initialize socket\n");
        pthread_exit(NULL);
    }


    SERVER_SOCKET = socket(AF_INET, SOCK_STREAM, 0);

    if (SERVER_SOCKET == INVALID_SOCKET) {
        printf("[server] Failed to create socket\n");
        pthread_exit(NULL);
    }

    printf("[server] Server socket initialization is OK\n");

    struct in_addr ip_to_num;

    iResult = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);

    if (iResult <= 0) {
        printf("[server] Error in IP translation to special numeric format\n");
        pthread_exit(NULL);
    }

    struct sockaddr_in serverInfo;
    ZeroMemory(&serverInfo, sizeof(serverInfo));

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr = ip_to_num;
    serverInfo.sin_port = htons(DEFAULT_PORT);

    iResult = bind(SERVER_SOCKET, (struct sockaddr*)&serverInfo, sizeof(serverInfo));

    if (iResult != 0) {
        printf("[server] Error Socket binding to server info\n");
        pthread_exit(NULL);
    }

    printf("[server] Binding socket to Server info is OK\n");

    iResult = listen(SERVER_SOCKET, 15);

    if (iResult != 0) {
        printf("[server] Can't start to listen to\n");
    }

    initializeServer();
    serverLoop();

    pthread_mutex_destroy(&GENERAL_AUTH_MUTEX);
    closesocket(SERVER_SOCKET);
    WSACleanup();
    return 0;
}