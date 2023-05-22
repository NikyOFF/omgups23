#include "handle-get-game-servers.rpc-command.h"

int handleGetGameServersRpcCommand(
        Connection* connection,
        Binary* clientPacket
) {
    Binary_clear(clientPacket);

    Binary_writeSizeT(clientPacket, GAME_SERVER_VEC.length);

    int index;
    GameServer* gameServer;

    vec_foreach(&GAME_SERVER_VEC, gameServer, index) {
        Binary_writeGameServer(clientPacket, gameServer);
    }

    if (send(connection->socket, clientPacket->buffer, clientPacket->writeIndex, 0) == -1) {
        return -1;
    }

    return 0;
}