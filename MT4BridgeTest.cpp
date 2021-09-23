#include <unistd.h>
#include "stdio.h"
#include "library.h"

int main() {
    printf("Starting Test ...");

    rabbitInit();

    while(true) {
        char buf[256];

        int count = rabbitMessagesAvailable();
        printf("rabbitMessagesAvailable: %u\n", count);

        if (count>0) {
            int result = rabbitGetMessage(buf);
            if (result == 0) {
                printf("rabbitCheckForMessage: failure --> result = 0\n");
            } else {
                printf("rabbitCheckForMessage: result = %u --> %s\n", result, buf);
            }
        }

        sleep(30);
    }

    rabbitDeinit();
    return 0;
}