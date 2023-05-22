#ifndef HANDLE_CREATE_GAME_SERVER_RPC_COMMAND_H
#define HANDLE_CREATE_GAME_SERVER_RPC_COMMAND_H

#include "../globals.h"
#include "../utils/utils.h"
#include "../connection/connection.h"
#include "../game-server/game-server.h"

#define CREATE_GAME_SERVER_RPC_COMMAND "rpc:create_game_server"

int handleCreateGameServerRpcCommand(
        Connection* connection,
        Binary* clientPacket
);

#endif //HANDLE_CREATE_GAME_SERVER_RPC_COMMAND_H
