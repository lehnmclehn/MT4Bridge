#ifndef TESTLIB_LIBRARY_H
#define TESTLIB_LIBRARY_H

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

typedef struct {
    int value;
    char str[256];
} MyType;

EXTERN_DLL_EXPORT int getInteger();

EXTERN_DLL_EXPORT void getString(char* buf);

EXTERN_DLL_EXPORT void getStruct(MyType* data);

EXTERN_DLL_EXPORT void sendMessage(char* msg);
#endif //TESTLIB_LIBRARY_H
