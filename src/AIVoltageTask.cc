#include "AIVoltageTask.h"
#include "error.h"

Persistent<Function> AIVoltageTask::constructor;

AIVoltageTask::AIVoltageTask() {

    buffArray = NULL;
    taskHandle = 0;
}

AIVoltageTask::~AIVoltageTask() {

    if (buffArray != NULL) {
        delete [] buffArray;
    }

    if (taskHandle != 0) {
        DAQmxStopTask(taskHandle);
        DAQmxClearTask(taskHandle);
    }
}


void AIVoltageTask::Init(Handle<Object> exports) {
    Isolate* isolate = Isolate::GetCurrent();

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "AIVoltageTask"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    NODE_SET_PROTOTYPE_METHOD(tpl, "start", Start);
    NODE_SET_PROTOTYPE_METHOD(tpl, "stop", Stop);
    NODE_SET_PROTOTYPE_METHOD(tpl, "clear", Clear);
    NODE_SET_PROTOTYPE_METHOD(tpl, "done", IsDone);
    NODE_SET_PROTOTYPE_METHOD(tpl, "read", Read);
    NODE_SET_PROTOTYPE_METHOD(tpl, "canRead", CanRead);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(String::NewFromUtf8(isolate, "AIVoltageTask"), tpl->GetFunction());

}

// DAQmxCreateTask
// DAQmxCreateAIVoltageChan
// DAQmxCfgSampClkTiming
// DAQmxCfgDigEdgeStartTrig
// DAQmxCfgAnlgEdgeStartTrig
// DAQmxSetAnlgEdgeStartTrigHyst
void AIVoltageTask::New(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.IsConstructCall()) {
        // Invoked as constructor: `new AIVoltageTask(...)`

        Local<Object> specs = args[0]->ToObject();


        AIVoltageTask* obj = new AIVoltageTask();
        obj->Wrap(args.This());

        // DAQmxCreateTask


        std::string taskName = "";

        if (!specs->Get(String::NewFromUtf8(isolate, "name"))->IsUndefined())
        {
            taskName = std::string(*v8::String::Utf8Value(
                specs->Get(String::NewFromUtf8(isolate, "name"))->ToString()));
        }

        throwIfFailed(isolate, DAQmxCreateTask(taskName.c_str(), &(obj->taskHandle)));

        Local<Array> aichannels = Local<Array>::Cast(specs->Get(String::NewFromUtf8(isolate, "channels")));

        obj->numChannels = aichannels->Length();

        for(int i = 0; i < obj->numChannels; i++) {
            // DAQmxCreateAIVoltageChan

            Local<Object> channel = aichannels->Get(i)->ToObject();
            Local<Value> tmpValue;


            std::string physicalChannel(*v8::String::Utf8Value(
                channel->Get(String::NewFromUtf8(isolate, "physicalChannel"))->ToString()));

            std::string nameToAssignToChannel = "";

            if (!channel->Get(String::NewFromUtf8(isolate, "assignName"))->IsUndefined())
            {
                nameToAssignToChannel = std::string(*v8::String::Utf8Value(
                    channel->Get(String::NewFromUtf8(isolate, "assignName"))->ToString()));
            }


            float64 minVal = -10.0;

            tmpValue = channel->Get(String::NewFromUtf8(isolate, "minVal"));

            if (!tmpValue->IsUndefined())
            {
                minVal = (float64) tmpValue->NumberValue();
            }


            float64 maxVal = 10.0;

            tmpValue = channel->Get(String::NewFromUtf8(isolate, "maxVal"));

            if (!tmpValue->IsUndefined())
            {
                maxVal = (float64) tmpValue->NumberValue();
            }


            int32 scale = DAQmx_Val_Volts;
            std::string customScaleName;
            const char* customScale = NULL;

            tmpValue = channel->Get(String::NewFromUtf8(isolate, "customScale"));

            if (!tmpValue->IsUndefined())
            {
                customScaleName = std::string(*v8::String::Utf8Value(tmpValue->ToString()));
                scale = DAQmx_Val_FromCustomScale;
                customScale = customScaleName.c_str();
            }


            throwIfFailed(isolate, DAQmxCreateAIVoltageChan(
                obj->taskHandle,
                physicalChannel.c_str(),
                nameToAssignToChannel.c_str(),
                DAQmx_Val_Cfg_Default,
                minVal,
                maxVal,
                scale,
                customScale));
        }

        //DAQmxCfgSampClkTiming

        Local<Object> timing = specs->Get(String::NewFromUtf8(isolate, "sampleTiming"))->ToObject();


        std::string source = "";

        if (!timing->Get(String::NewFromUtf8(isolate, "source"))->IsUndefined())
        {
            source = std::string(*v8::String::Utf8Value(
                timing->Get(String::NewFromUtf8(isolate, "source"))->ToString()));
        }

        if (source.compare("internal") == 0) {
            source = "";
        }

        float64 rate = (float64) timing->Get(String::NewFromUtf8(isolate, "rate"))->NumberValue();

        int32 activeEdge;


        std::string edgeTriggerString = "rising";

        if (!timing->Get(String::NewFromUtf8(isolate, "triggerSlope"))->IsUndefined())
        {
            edgeTriggerString = std::string(*v8::String::Utf8Value(
                timing->Get(String::NewFromUtf8(isolate, "triggerSlope"))->ToString()));
        }

        if (edgeTriggerString.compare("rising") == 0) {
            activeEdge = DAQmx_Val_Rising;
        }else if (edgeTriggerString.compare("falling") == 0){
            activeEdge = DAQmx_Val_Falling;
        }else{

            isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate, "trigger slope must be either rising or falling.")));
        }

        int32 sampleMode;

        std::string sampleModeString = "finite";

        if (!timing->Get(String::NewFromUtf8(isolate, "sampleMode"))->IsUndefined())
        {
            sampleModeString = std::string(*v8::String::Utf8Value(
                timing->Get(String::NewFromUtf8(isolate, "sampleMode"))->ToString()));
        }

        if (sampleModeString.compare("finite") == 0) {
            sampleMode = DAQmx_Val_FiniteSamps;
        }else if (sampleModeString.compare("continuous") == 0){
            sampleMode = DAQmx_Val_ContSamps;
        }else if (sampleModeString.compare("single") == 0){
            sampleMode = DAQmx_Val_HWTimedSinglePoint;
        }else{

            isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate, "sample mode must be either finite, continuous, or single.")));
        }

        obj->numSampsPerChan = (uInt64) timing->Get(String::NewFromUtf8(isolate, "samplesPerChannel"))->NumberValue();

        throwIfFailed(isolate, DAQmxCfgSampClkTiming(
            obj->taskHandle,
            source.c_str(),
            rate,
            activeEdge,
            sampleMode,
            obj->numSampsPerChan));


        // according to DAQmx docs the internal buffer size is at least enough to hold all samples requested
        // but we won't request more than we requested, so just make it be enough
        obj->buffSize = obj->numSampsPerChan * obj->numChannels;
        obj->buffArray = new double[obj->buffSize];



        if (!specs->Get(String::NewFromUtf8(isolate, "startTiming"))->IsUndefined()){

            Local<Object> trigger = specs->Get(String::NewFromUtf8(isolate, "startTiming"))->ToObject();

            std::string triggerType(*v8::String::Utf8Value(trigger->Get(String::NewFromUtf8(isolate, "type"))->ToString()));

            if (triggerType.compare("digital")) {
                //DAQmxCfgDigEdgeStartTrig

                std::string source(*v8::String::Utf8Value(trigger->Get(String::NewFromUtf8(isolate, "source"))->ToString()));

                int32 activeEdge;

                std::string edgeTriggerString = "rising";

                if (!trigger->Get(String::NewFromUtf8(isolate, "triggerSlope"))->IsUndefined())
                {
                    edgeTriggerString = std::string(*v8::String::Utf8Value(
                        trigger->Get(String::NewFromUtf8(isolate, "triggerSlope"))->ToString()));
                }

                if (edgeTriggerString.compare("rising") == 0) {
                    activeEdge = DAQmx_Val_Rising;
                }else if (edgeTriggerString.compare("falling") == 0){
                    activeEdge = DAQmx_Val_Falling;
                }else{

                    isolate->ThrowException(Exception::TypeError(
                            String::NewFromUtf8(isolate, "trigger slope must be either rising or falling.")));
                }

                throwIfFailed(isolate, DAQmxCfgDigEdgeStartTrig(
                    obj->taskHandle,
                    source.c_str(),
                    activeEdge));

            }else if (triggerType.compare("analog")) {
                // DAQmxCfgAnlgEdgeStartTrig

                std::string source(*v8::String::Utf8Value(trigger->Get(String::NewFromUtf8(isolate, "source"))->ToString()));

                int32 activeEdge;

                std::string edgeTriggerString = "rising";

                if (!trigger->Get(String::NewFromUtf8(isolate, "triggerSlope"))->IsUndefined())
                {
                    edgeTriggerString = std::string(*v8::String::Utf8Value(
                        trigger->Get(String::NewFromUtf8(isolate, "triggerSlope"))->ToString()));
                }

                if (edgeTriggerString.compare("rising") == 0) {
                    activeEdge = DAQmx_Val_Rising;
                }else if (edgeTriggerString.compare("falling") == 0){
                    activeEdge = DAQmx_Val_Falling;
                }else{

                    isolate->ThrowException(Exception::TypeError(
                            String::NewFromUtf8(isolate, "trigger slope must be either rising or falling.")));
                }


                float64 triggerLevel = trigger->Get(String::NewFromUtf8(isolate, "triggerLevel"))->NumberValue();


                throwIfFailed(isolate, DAQmxCfgAnlgEdgeStartTrig(
                    obj->taskHandle,
                    source.c_str(),
                    activeEdge,
                    triggerLevel));

                if (!trigger->Get(String::NewFromUtf8(isolate, "hysteresis"))->IsUndefined()) {
                    // DAQmxSetAnlgEdgeStartTrigHyst

                    float64 hysteresis = trigger->Get(String::NewFromUtf8(isolate, "hysteresis"))->NumberValue();

                    throwIfFailed(isolate, DAQmxSetAnlgEdgeStartTrigHyst(
                        obj->taskHandle,
                        hysteresis));
                }
            }else{

                isolate->ThrowException(Exception::TypeError(
                        String::NewFromUtf8(isolate, "trigger type must be either digital or analog.")));
            }
        }

        args.GetReturnValue().Set(args.This());
    } else {
        // Invoked as plain function `ReadBuffer64(...)`, turn into construct call.
        const int argc = 1;
        Local<Value> argv[argc] = { args[0] };
        Local<Function> cons = Local<Function>::New(isolate, constructor);
        args.GetReturnValue().Set(cons->NewInstance(argc, argv));
    }


}

// DAQmxStartTask
void AIVoltageTask::Start(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    AIVoltageTask* myTask = node::ObjectWrap::Unwrap<AIVoltageTask>(args.Holder());

    throwIfFailed(isolate, DAQmxStartTask(myTask->taskHandle));
}

// DAQmxStopTask
void AIVoltageTask::Stop(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    AIVoltageTask* myTask = node::ObjectWrap::Unwrap<AIVoltageTask>(args.Holder());

    throwIfFailed(isolate, DAQmxStopTask(myTask->taskHandle));
}

// DAQmxClearTask
void AIVoltageTask::Clear(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    AIVoltageTask* myTask = node::ObjectWrap::Unwrap<AIVoltageTask>(args.Holder());

    throwIfFailed(isolate, DAQmxClearTask(myTask->taskHandle));
}

// DAQmxIsTaskDone
void AIVoltageTask::IsDone(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    AIVoltageTask* myTask = node::ObjectWrap::Unwrap<AIVoltageTask>(args.Holder());
    bool32 isDone = FALSE;

    throwIfFailed(isolate, DAQmxIsTaskDone(myTask->taskHandle, &isDone));


    args.GetReturnValue().Set(Boolean::New(isolate, isDone == TRUE));
}

void AIVoltageTask::CanRead(const FunctionCallbackInfo<Value>& args) {

    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);


    AIVoltageTask* myTask = node::ObjectWrap::Unwrap<AIVoltageTask>(args.Holder());

    int requestedSamples = myTask->numSampsPerChan;

    if (args.Length() > 0) {
        requestedSamples = args[0]->IntegerValue();
    }


    int32 sampsPerChanCanRead = 0;

    throwIfFailed(isolate, DAQmxGetReadAttribute(
        myTask->taskHandle,
        DAQmx_Read_AvailSampPerChan,
        &sampsPerChanCanRead
    ));

    args.GetReturnValue().Set(Boolean::New(isolate, sampsPerChanCanRead >= requestedSamples));

}

//DAQmxReadAnalogF64
void AIVoltageTask::Read(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    AIVoltageTask* myTask = node::ObjectWrap::Unwrap<AIVoltageTask>(args.Holder());

    float64 timeout = (float64) args[0]->NumberValue();


    int32 sampsPerChanRead = 0;


    throwIfFailed(isolate, DAQmxReadAnalogF64(
        myTask->taskHandle,
        myTask->numSampsPerChan,
        timeout,
        DAQmx_Val_GroupByChannel,
        myTask->buffArray,
        myTask->buffSize,
        &sampsPerChanRead,
        NULL
    ));

    Local<Array> channels = Array::New(isolate, myTask->numChannels);


    for(int i = 0; i < myTask->numChannels; i++) {
        Local<Array> channelData = Array::New(isolate, sampsPerChanRead);

        for(int j = 0; j < sampsPerChanRead; j++) {
            channelData->Set(j, Number::New(isolate, myTask->buffArray[i * sampsPerChanRead + j]));
        }

        channels->Set(i, channelData);
    }


    args.GetReturnValue().Set(channels);

}
