#ifndef error_h
#define error_h

#include <node.h>
#include <NIDAQmx.h>

using namespace v8;

void throwIfFailed (Isolate* isolate, int32 err) {

    if (DAQmxFailed(err)) {
        char errBuff[2048] = {'\0'};
        DAQmxGetExtendedErrorInfo(errBuff, 2048);

        isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, errBuff)));
    }
}

#endif
