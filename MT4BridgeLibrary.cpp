#include "library.h"

#include "AMQPConnection.h"
#include "string.h"

AMQPConnection conn;

EXTERN_DLL_EXPORT void rabbitInit() {
    conn.init();

    // starts the server listing
    conn.startListen();
}

EXTERN_DLL_EXPORT void rabbitDeinit() {
    conn.deinit();
}

EXTERN_DLL_EXPORT void rabbitSendMessage(char *msg) {
    TDHTransferHeader header;
    header.version = 1;
    header.type = 0;
    strcpy(header.msg, msg);

    conn.send(header);
}

EXTERN_DLL_EXPORT int rabbitGetMessage(char *msg) {

    int count = conn.messagesAvailable();
    if (count > 0) {
        TDHTransferHeader header;
        conn.getMessage(&header);
        strcpy(msg,header.msg);
        return 1;
    } else {
        return 0;
    }
}

EXTERN_DLL_EXPORT int rabbitMessagesAvailable() {
    int count = conn.messagesAvailable();
    return count;
}


/* ******************
 * DEPRECATED
 */
EXTERN_DLL_EXPORT void getString(char *buf) {
    strcpy(buf, "Hello, World!");
}

EXTERN_DLL_EXPORT void getStruct(MyType *data) {
    data->value = 99;
    strcpy(data->str, "It worked!");
}


