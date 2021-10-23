#ifndef TESTLIB_LIBRARY_H
#define TESTLIB_LIBRARY_H

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)


/*
 *  Command structure sent from THD --> MT4
 */
typedef struct {
    char id[256];
    char action[32];
    char symbol[32];
    long dir;

    // default trade commands
    float count;
    float takeprofit;
    float stopp;

    // info commands to visualize trades
    long  entrytime;
    float entryprice;
    long  exittime;
    float exitprice;

} TDHCmd;


/*
 *  Command response structure sent from MT4 --> TDH
 */
typedef struct {
    char id[256];
    char state[32];
} TDHCmdResponse;


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
 * sends a info bar to rabbit MQ
 */
EXTERN_DLL_EXPORT void rabbitSendBar(TDHBar* bar);

/*
 * checks if a new message is available / returns 0 if not
 */
EXTERN_DLL_EXPORT int rabbitGetMessage(TDHCmd* msg);

/*
 * sends a response to a command
 */
EXTERN_DLL_EXPORT void rabbitSendCmdResponse(TDHCmdResponse* response);

/*
 * returns the count of available messages
 */
EXTERN_DLL_EXPORT int rabbitMessagesAvailable();

#endif //TESTLIB_LIBRARY_H
