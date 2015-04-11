
/**
    nodedaqmx.cc
*/

#include <node.h>

#include "AIVoltageTask.h"


using namespace v8;


void init(Handle<Object> exports) {
    AIVoltageTask::Init(exports);


}

NODE_MODULE(nodedaqmx, init)
