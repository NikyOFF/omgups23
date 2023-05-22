#ifndef HANDLE_CONNECT_TO_SERVER_RPC_COMMAND_H
#define HANDLE_CONNECT_TO_SERVER_RPC_COMMAND_H

#include "../globals.h"
#include "../utils/utils.h"
#include "../connection/connection.h"
#include "../game-server/game-server.h"

#define CONNECT_TO_SERVER_RPC_COMMAND "rpc:connect_to_server"

int handleConnectToServerRpcCommand(
        Connection* connection,
        Binary* clientPacket
);

#endif //HANDLE_CONNECT_TO_SERVER_RPC_COMMAND_H
