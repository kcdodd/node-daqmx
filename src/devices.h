#ifndef devices_h
#define devices_h

#include <NIDAQmx.h>
#include <string>
#include <list>
#include "error.h"

using namespace v8;

void devices(const FunctionCallbackInfo<Value>& args) {

    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    char buffer[2048];

    throwIfFailed(isolate, DAQmxGetSysDevNames(buffer, 2048));



    std::string s(buffer);
    std::string delimiter = ", ";

    size_t pos = 0;
    std::string token;
    std::list<std::string> tokens;

    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }

    // get last token
    tokens.push_back(s);

    int length = tokens.size();

    Local<Array> devArray = Array::New(isolate, length);

    for(int i = 0; i < length; i++) {
        Local<Object> device = Object::New(isolate);

        const char* devname = tokens.front().c_str();

        // set name of device
        device->Set(
            String::NewFromUtf8(isolate, "name", String::kInternalizedString),
            String::NewFromUtf8(isolate, devname));

        // get more information about each device

        throwIfFailed(isolate, DAQmxGetDeviceAttribute(devname, DAQmx_Dev_ProductType, buffer, 2048));

        device->Set(
            String::NewFromUtf8(isolate, "productType", String::kInternalizedString),
            String::NewFromUtf8(isolate, buffer));

        devArray->Set(i, device);
        tokens.pop_front();
    }


    args.GetReturnValue().Set(devArray);

}

#endif
