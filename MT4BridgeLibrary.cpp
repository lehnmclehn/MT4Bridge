#include <iostream>
#include <json/json.h>
#include "library.h"
#include "AMQPConnection.h"

AMQPConnection conn;

EXTERN_DLL_EXPORT void rabbitInit() {
    conn.init();

    // starts the server listing
    conn.startListen();
}

EXTERN_DLL_EXPORT void rabbitDeinit() {
    conn.deinit();
}

EXTERN_DLL_EXPORT void rabbitSendBar(TDHBar* bar) {
    conn.send(bar);
}

EXTERN_DLL_EXPORT int rabbitGetMessage(TDHCmd *cmd) {

    int count = conn.messagesAvailable();
    if (count > 0) {
        conn.getMessage(cmd);
        return 1;
    } else {
        return 0;
    }
}

EXTERN_DLL_EXPORT void rabbitSendCmdResponse(TDHCmdResponse* response) {
    conn.send(response);
}

EXTERN_DLL_EXPORT int rabbitMessagesAvailable() {
    int count = conn.messagesAvailable();
    return count;
}

