#include "library.h"

#include <string.h>

int callCounter = 0;

EXTERN_DLL_EXPORT int getInteger() {
    return callCounter++;
}

EXTERN_DLL_EXPORT void getString(char* buf) {
    strcpy (buf, "Hello, World!");
}

EXTERN_DLL_EXPORT void getStruct(MyType* data) {
    data->value = 99;
    strcpy (data->str, "It worked!");
}
