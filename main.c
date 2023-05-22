#include "server.h"

#pragma comment(lib, "Ws2_32.lib")


void connectionLoop(Connection* connection) {
    int iResult = 0;
    Binary* clientPacket = Binary_constructor(512);

    while (connection->socket != INVALID_SOCKET || iResult == 0) {
        Binary_clear(clientPacket); //clear binary before receive new data

        printfForConnection(connection, "Wait rpc command from client socket");
        iResult = recv(connection->socket, clientPacket->buffer, 512, 0); //receive data from socket

        //handle socket error or zero bytes
        if (iResult == SOCKET_ERROR || iResult == 0) {
            break;
        }

        connection->lastHealthCheck = time(NULL); //set last health check

        size_t rpcCommandSize; //size for rpc command string
        char* rpcCommandName = Binary_readString(clientPacket, &rpcCommandSize); //read "rpc command name" from binary


        printfForConnection(connection, "Handle rpc command from client socket: %s (%ib)", rpcCommandName, iResult);


        //Handle rpc commands

        if (strcmp(rpcCommandName, CREATE_GAME_SERVER_RPC_COMMAND) == 0) {
            iResult = handleCreateGameServerRpcCommand(connection, clientPacket);
            continue;
        }

        if (strcmp(rpcCommandName, CONNECT_TO_SERVER_RPC_COMMAND) == 0) {
            iResult = handleConnectToServerRpcCommand(connection, clientPacket);
            continue;
        }

        if (strcmp(rpcCommandName, GET_GAME_SERVERS_RPC_COMMAND) == 0) {
            iResult = handleGetGameServersRpcCommand(connection, clientPacket);
            continue;
        }
    }

    Binary_deconstructor(clientPacket); //deconstruct binary
}

void* onConnection(void* arg) {
    Connection* connection = (Connection *) arg;
    int iResult;

    connection->lastHealthCheck = time(NULL);

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
    connection->lastHealthCheck = time(NULL);
    pthread_mutex_unlock(&CONNECTIONS_MUTEX);

    printfForConnection(connection, "User \"%s\" connected!", connection->user->login);

    connectionLoop(connection);

    onDisconnect(connection);

    printfForConnection(connection, "User \"%s\" disconnected!", connection->user->login);
    pthread_exit(0);
}



#define SERVER_SOCKET_PING_PACKET_MESSAGE "SERVER_PING"
#define SERVER_SOCKET_PING_PACKET_MESSAGE_SIZE 12
#define SERVER_SOCKET_PING_PACKET_SIZE 20

#define CLIENT_SOCKET_PING_PACKET_MESSAGE "CLIENT_PONG"
#define CLIENT_SOCKET_PING_PACKET_SIZE 20

int sendHealthCheckPacket(Connection* connection) {
    Binary* binary = Binary_constructor(SERVER_SOCKET_PING_PACKET_SIZE);
    Binary_writeString(binary, SERVER_SOCKET_PING_PACKET_MESSAGE);

    int result = send(connection->socket, binary->buffer, binary->writeIndex, 0);

    if (result == SOCKET_ERROR) {
        free(binary->buffer);
        return -1;
    }

    Binary_clear(binary);

    result = recv(connection->socket, binary->buffer, binary->bufferSize, 0);

    if (result == SOCKET_ERROR) {
        free(binary->buffer);
        return -1;
    }

    if (result == CLIENT_SOCKET_PING_PACKET_SIZE) {
        size_t strSize;
        char *str = Binary_readString(binary, &strSize);

        if (strSize == SERVER_SOCKET_PING_PACKET_MESSAGE_SIZE && strcmp(str, CLIENT_SOCKET_PING_PACKET_MESSAGE) == 0) {
            free(binary->buffer);
            return 0;
        }
    }

    free(binary->buffer);
    return -1;
}

void* connectionsHealthCheckThread(void *arg) {
    int timestamp = time(NULL) + CONNECTIONS_CHECK_DELAY_IN_SECONDS;

    while (1) {
        long long currentTime = time(NULL);

        if (currentTime < timestamp) {
            continue;
        }

        timestamp = time(NULL) + CONNECTIONS_CHECK_DELAY_IN_SECONDS;

        int index;
        Connection* connection;

        vec_foreach(&CONNECTIONS_VEC, connection, index) {
            printf("[Connections health check THREAD] check socket %lli [%lli]\n", connection->socket, connection->lastHealthCheck);

            if (connection->connectionState == AUTHORIZATION_CONNECTION_STATE && currentTime - connection->lastHealthCheck > 60) {
                onDisconnect(connection);
                continue;
            }

            if (connection->connectionState == AUTHORIZED_CONNECTION_STATE, currentTime - connection->lastHealthCheck > 240) {
                onDisconnect(connection);
                continue;
            }
        }
    }

}

void* consoleInputThread(void *arg) {
    char input[256];

    while (1) {
        if (scanf("%256s", input) != 1) {
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

        iResult = pthread_create(&tid, &attr, onConnection, vec_last(&CONNECTIONS_VEC));

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