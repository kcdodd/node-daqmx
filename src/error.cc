#include "error.h"


void throwIfFailed (Isolate* isolate, int32 err) {

    if (DAQmxFailed(err)) {
        char errBuff[2048] = {'\0'};
        DAQmxGetExtendedErrorInfo(errBuff, 2048);

        isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, errBuff)));
    }
}
