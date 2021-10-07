#include <unistd.h>
#include <iostream>
#include "stdio.h"
#include "library.h"
#include "json/json.h"

void sendTestMessage() {
    TDHBar bar;

    strcpy(bar.symbol, "TEST");
    bar.period = 99;
    bar.digits = 3;
    bar.open = 99.9;
    bar.high = 99.9;
    bar.low = 99.9;
    bar.close = 99.9;
    bar.time = 1234567;

    rabbitSendBar(&bar);
}

void runServer() {
    printf("Starting Test ...");

    rabbitInit();
    sendTestMessage();

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
                printf("rabbitCheckForMessage: result = %u --> Id=%s | Action=%s | Symbol=%s | Count=%02f| Stopp=%02f\n", result,
                       cmd.id, cmd.action,
                       cmd.symbol, cmd.stopp);
                // rabbitSendMessage("Message ack: correct");
            }
        }

        sleep(1);
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