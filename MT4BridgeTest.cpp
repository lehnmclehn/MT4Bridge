#include <unistd.h>
#include <iostream>
#include "stdio.h"
#include "library.h"
#include "json/json.h"


void runServer() {
    printf("Starting Test ...");

    rabbitInit();
    // rabbitSendMessage("Testprogram ist up and running ...");

    while (true) {
        int count = rabbitMessagesAvailable();
        printf("rabbitMessagesAvailable: %u\n", count);

        if (count > 0) {
            TDHCmd cmd;
            int result = rabbitGetMessage(&cmd);
            if (result == 0) {
                printf("rabbitCheckForMessage: failure --> result = 0\n");
                // rabbitSendMessage("Message ack: failure");
            } else {
                printf("rabbitCheckForMessage: result = %u --> Action=%s | Tradeable=%s | msg=%s\n", result, cmd.action,
                       cmd.tradeable, cmd.msg);
                // rabbitSendMessage("Message ack: correct");
            }
        }

        sleep(30);
    }

    rabbitDeinit();
}

void jsonTestMessage() {
    std::stringstream jsonText(R"({ "encoding" : "UTF-8", "name" : "alexander" })");
    Json::Value root;
    jsonText >> root;
    std::cout << "Mein Resultat: " << root.get("name", "n/a").asString();
}

int main() {
    // jsonTestMessage();
    runServer();
    return 0;
}