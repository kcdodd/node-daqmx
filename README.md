# node-daqmx

I wanted to combine National Instruments data acquisition hardware with Node.js. This is not a 1-to-1 wrapper of the DAQmx c api. I have re-interpreted some of the structures, but tried to be faithful to the original concepts. If you are familiar with DAQmx, a task in the c api is a generalized object, which has to be configured by adding or setting various properties and methods to accomplish anything. I took a more narrow rout with each type of task confined to a separate object.

## Pre-requisits

- Everything needed to build Node.js addons.
- Latest [National Instruments DAQmx drivers](http://www.ni.com/download/ni-daqmx-14.5/5212/en/).
- Either a physical NI device, or a virtual device created using their Measurement and Automation Explorer.

## Devices

To get a list of currently available devices.

```JavaScript
var devs = daqmx.devices();

console.log(devs);
```

returns an array of objects of each device with properties: E.G.
```JavaScript
[{
    name: "Dev1",
    productType: "USB-6000"
},
{
    name: "Dev2",
    productType: "PCI-6071E"
}]
```

## AI Voltage Task

The minimal configuration to acquire a sample must have defined the channels to acquire on, and the timing of the samples. Specifying the channels separatly as an array will cause the read to return a 2D array, where the first index is the channels (in the same order), and second index is sample #. There can typically be only one task of each type running per physical device because of how the signals are digitized. There's usually only 1 ADC (or DAC), which is split between the channels.

```JavaScript
var daqmx = require('pathto/nodedaqmx');

// read 10 samples from 2 channels at 10,000Hz sample rate
var task = new daqmx.AIVoltageTask({

    channels: [
        {physicalChannel: "Dev1/ai0"},
        {physicalChannel: "Dev1/ai1"}
    ],

    sampleTiming: {
        rate: 10000.0,
        samplesPerChannel: 10
    }
});


task.start();

var x = task.read(); // infinite timeout

//var x = task.read(10.0); // 10 second timeout

console.log(x);
```

The reading of samples can also be done in the Node.js io loop so that it doesn't block the program execution.

```JavaScript
// do a read asyncronously
var readAsync = function(callback) {

    process.nextTick(function(){
        // call canRead() tests if all samples can be read
        if (task.canRead()) {
            var data = task.read(0.0); // should be immediate

            callback(data);
        }else{
            // not ready so wait until next tick
            readAsync(callback);
        }
    });
};

readAsync(function(data){
    console.log(data);
});
```

The AIVoltageTask can be configured with most of the abilities available. These examples are translated from the examples supplied in the DAQmx documentation.

To do continuous sampling of a single channel using the internal clock and a digital start trigger:

```JavaScript
var task = new daqmx.AIVoltageTask({

    name: "myAIVoltageTask", // optional name for task, default ""

    channels: [{ // array of objects specifying the channels to acquire
        physicalChannel: "Dev1/ai0", // physical device and channel name
        assignName: "someChannelName" // optional name for channel
        minVal: -10.0, // optional, default -10.0
        maxVal: 10.0 // optional, default 10.0
    }],

    sampleTiming: { // timing for individual samples
        source: "internal", // optional default 'internal'
        rate: 10000.0, // sample rate in Hz
        triggerSlope: "falling", // optional, default rising
        sampleMode: "continuous", // optional, default finite
        samplesPerChannel: 10 // max number of samples to read per round
    },

    startTiming: { // optional timing for when to start the task
        type: "digital", // analog, or digital
        source: "/Dev1/PFI0",
        triggerSlope: "falling" // optional, default rising
    }
});
```

To do a finite sample of a single channel using an external clock, and an external analog start trigger including hysteresis.

```JavaScript
var task = new daqmx.AIVoltageTask({

    name: "myAIVoltageTask",

    channels: [{
        physicalChannel: "Dev1/ai0",
        minVal: -10.0,
        maxVal: 10.0
    }],

    sampleTiming: {
        source: "/Dev1/PFI7",
        rate: 10000.0,
        triggerSlope: "rising",
        sampleMode: "finite",
        samplesPerChannel: 10
    },

    startTiming: {
        type: "analog",
        source: "APFI0",
        triggerSlope: "rising", // optional, default rising
        triggerLevel: 1.0, // analog trigger only in volts
        hysteresis: 1.0 // optional for analog trigger only, default 0.0
    }
});
```
