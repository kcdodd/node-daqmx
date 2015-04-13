#ifndef daqmxerror_h
#define daqmxerror_h

#include <node.h>
#include <NIDAQmx.h>

using namespace v8;

void throwIfFailed (Isolate* isolate, int32 err);

#endif
