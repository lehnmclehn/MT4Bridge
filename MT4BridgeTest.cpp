#include <unistd.h>
#include "stdio.h"
#include "library.h"

int main() {
    printf("Starting Test ...");

    rabbitInit();
    rabbitSendMessage("Testprogram ist up and running ...");

    while(true) {
        char buf[256];

        int count = rabbitMessagesAvailable();
        printf("rabbitMessagesAvailable: %u\n", count);

        if (count>0) {
            int result = rabbitGetMessage(buf);
            if (result == 0) {
                printf("rabbitCheckForMessage: failure --> result = 0\n");
                rabbitSendMessage("Message ack: failure");
            } else {
                printf("rabbitCheckForMessage: result = %u --> %s\n", result, buf);
                rabbitSendMessage("Message ack: correct");
            }
        }

        sleep(30);
    }

    rabbitDeinit();
    return 0;
}