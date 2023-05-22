#include "handle-create-game-server.rpc-command.h"


int handleCreateGameServerRpcCommand(
    Connection* connection,
    Binary* clientPacket
) {
    size_t serverNameSize;
    char* serverName = Binary_readString(clientPacket, &serverNameSize);

    GameServer* gameServer = GameServer_constructor(time(NULL), serverName, connection->socket);

    //add game server to vector
    pthread_mutex_lock(&GAME_SERVERS_MUTEX);
    vec_push(&GAME_SERVER_VEC, gameServer);
    pthread_mutex_unlock(&GAME_SERVERS_MUTEX);

    Binary_clear(clientPacket);

    Binary_writeInt(clientPacket, 0); //write success result to binary
    Binary_writeGameServer(clientPacket, gameServer); //write game server to binary

    printfForConnection(connection, "Create server: %s (%zu)\"", gameServer->serverName, gameServer->id);

    //send data
    if (send(connection->socket, clientPacket->buffer, clientPacket->writeIndex, 0) == -1) {
        return -1;
    }

    //wait client ready to connect signal
    pthread_cond_wait(&gameServer->_connectionRequest, &gameServer->_serverMutex);
//    printfForConnection(connection, "awaited signal 1\"");

    //remove game server from vector
    pthread_mutex_lock(&GAME_SERVERS_MUTEX);
    vec_remove(&GAME_SERVER_VEC, gameServer);
    pthread_mutex_unlock(&GAME_SERVERS_MUTEX);

    //signal server ready to finish client connection
//    printfForConnection(connection, "before signal 2\"");
//    pthread_cond_signal(&gameServer->_connectionRequest);
//    printfForConnection(connection, "after signal 2\"");

    Binary_deconstructor(clientPacket);
    return 0;
}