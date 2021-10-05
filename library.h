#ifndef TESTLIB_LIBRARY_H
#define TESTLIB_LIBRARY_H

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)


/*
 *  Command structure sent from THD --> MT4
 */
typedef struct {
    long time;
    char action[32];
    char tradeable[32];
    char msg[256];
} TDHCmd;


/*
 *  Bar structure sent from MT4 --> TDH
 */
typedef struct {
    long time;
    char symbol[32];
    long period;
    long digits;
    float high;
    float open;
    float low;
    float close;
} TDHBar;


/*
 * initializes the AMQP Library
 */
EXTERN_DLL_EXPORT void rabbitInit();

/*
 * deinitializes the AMQP Library
 */
EXTERN_DLL_EXPORT void rabbitDeinit();

/*
 * sends a text message to rabbit MQ
 */
EXTERN_DLL_EXPORT void rabbitSendMessage(TDHBar* bar);

/*
 * checks if a new message is available / returns 0 if not
 */
EXTERN_DLL_EXPORT int rabbitGetMessage(TDHCmd* msg);

/*
 * returns the count of available messages
 */
EXTERN_DLL_EXPORT int rabbitMessagesAvailable();

#endif //TESTLIB_LIBRARY_H
