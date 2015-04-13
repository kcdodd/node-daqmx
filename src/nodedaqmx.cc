
/**
    nodedaqmx.cc
*/

#include <node.h>

#include "AIVoltageTask.h"
#include "devices.h"

using namespace v8;


void init(Handle<Object> exports) {
    AIVoltageTask::Init(exports);

    NODE_SET_METHOD(exports, "devices", devices);

}

NODE_MODULE(nodedaqmx, init)
