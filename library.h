#ifndef TESTLIB_LIBRARY_H
#define TESTLIB_LIBRARY_H

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

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
EXTERN_DLL_EXPORT void rabbitSendMessage(char* msg);

/*
 * checks if a new message is available / returns 0 if not
 */
EXTERN_DLL_EXPORT int rabbitGetMessage(char* msg);

/*
 * returns the count of available messages
 */
EXTERN_DLL_EXPORT int rabbitMessagesAvailable();



/*
 *  old test functions
 */
typedef struct {
    int value;
    char str[256];
} MyType;

EXTERN_DLL_EXPORT void getString(char* buf);

EXTERN_DLL_EXPORT void getStruct(MyType* data);


#endif //TESTLIB_LIBRARY_H
