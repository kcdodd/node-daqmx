#ifndef AIVoltageTask_h
#define AIVoltageTask_h


#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include <NIDAQmx.h>

using namespace v8;

class AIVoltageTask : public node::ObjectWrap {
private:
    double* buffArray;
    int buffSize;

    TaskHandle taskHandle;
    int32 numChannels;
    int32 numSampsPerChan;

    Persistent< Function > readCallback;

public:


    static void Init(v8::Handle<v8::Object> exports);

private:

    explicit AIVoltageTask();

    ~AIVoltageTask();

    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void Start(const FunctionCallbackInfo<Value>& args);
    static void Stop(const FunctionCallbackInfo<Value>& args);
    static void Clear(const FunctionCallbackInfo<Value>& args);
    static void IsDone(const FunctionCallbackInfo<Value>& args);
    static void Read(const FunctionCallbackInfo<Value>& args);
    static void CanRead(const FunctionCallbackInfo<Value>& args);

    static v8::Persistent<v8::Function> constructor;

};

#endif
