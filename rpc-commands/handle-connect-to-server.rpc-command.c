#include "handle-connect-to-server.rpc-command.h"

int handleConnectToServerRpcCommand(
        Connection* connection,
        Binary* clientPacket
) {

    size_t gameServerId;
    Binary_readSizeT(clientPacket, &gameServerId);

    GameServer* gameServer = getGameServerById(gameServerId); //find game server by id

    int iResult = gameServer == NULL;

    Binary_clear(clientPacket);
    Binary_writeInt(clientPacket, iResult);

    //send find result to client
    if (send(connection->socket, clientPacket->buffer, clientPacket->writeIndex, 0) == -1) {
        return -1;
    }

    if (iResult != 0) {
        printfForConnection(connection, "Cannot connect to server with id: %zu", gameServer->serverName, gameServerId);
        return 0;
    }

//    printfForConnection(connection, "before signal 1\"");
    pthread_cond_signal(&gameServer->_connectionRequest); //signal client ready to connect
//    printfForConnection(connection, "after signal 1\"");

    printfForConnection(connection, "Connect to server: %s (%zu) [client %llu]", gameServer->serverName, gameServer->id, gameServer->_owner);

    Binary_clear(clientPacket);
    Binary_writeUser(clientPacket, connection->user);

//    pthread_cond_wait(&gameServer->_connectionRequest, &gameServer->_serverMutex); //wait server ready to finish connection signal
//    printfForConnection(connection, "awaited signal 2\"");
//
//    printfForConnection(connection, "Send user info to server: %s (%zu) [client %llu]", gameServer->serverName, gameServer->id, gameServer->_owner);

    //send data to owner
    if (send(gameServer->_owner, clientPacket->buffer, clientPacket->writeIndex, 0) == -1) {
        return -1;
    }

    return 0;
}