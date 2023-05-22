#ifndef HANDLE_GET_GAME_SERVERS_RPC_COMMAND_H
#define HANDLE_GET_GAME_SERVERS_RPC_COMMAND_H

#include "../globals.h"
#include "../utils/utils.h"
#include "../connection/connection.h"
#include "../game-server/game-server.h"

#define GET_GAME_SERVERS_RPC_COMMAND "rpc:get_game_servers"

int handleGetGameServersRpcCommand(
        Connection* connection,
        Binary* clientPacket
);

#endif //HANDLE_GET_GAME_SERVERS_RPC_COMMAND_H
