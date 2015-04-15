# node-daqmx

I wanted to combine National Instruments data acquisition hardware with Node.js. This is not a 1-to-1 wrapper of the DAQmx C api. I have re-interpreted some of the structures to make using DAQmx a little easier, but tried to be faithful to the original concepts.

If you need the full api, considering using [node-ffi](https://github.com/node-ffi/node-ffi) to load nicaiu.dll (from the daqmx driver installation) to access all the C API functions.

## Pre-requisits

- Everything needed to build Node.js addons.
- Latest [National Instruments DAQmx drivers](http://www.ni.com/download/ni-daqmx-14.5/5212/en/).
- Either a physical NI device, or a virtual device created using their Measurement and Automation Explorer.

The following examples assume the addon is loaded into the variable 'daqmx'.

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
// read 10 samples from 2 channels at 10,000Hz sample rate
var task = new daqmx.AIVoltageTask({

    device: "Dev1",

    channels: [{terminal: "ai0"},{terminal: "ai3"}],

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

Alternatly, if the terminals are consecutive, they can be specified in a single input. E.G. the following will acquire from 4 terminals (ai0, ai1, ai2, ai3).

```JavaScript
var task = new daqmx.AIVoltageTask({

    device: "Dev1",

    channels: [{terminal: "ai0:3"}],

    sampleTiming: {
        rate: 10000.0,
        samplesPerChannel: 10
    }
});
```

The reading of samples can also be done in the Node.js io loop so that it doesn't block the program execution.

```JavaScript
// do a read asyncronously
var readAsync = function(callback) {

    process.nextTick(function(){
        // call canRead() tests if all samples can be read
        if (task.canRead()) {
            var data = task.read(0.0); // should read immediatly

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

    device: "Dev1", // physical device name

    channels: [{ // array of objects specifying the channels to acquire
        terminal: "ai0", // physical terminal name
        assignName: "someChannelName" // optional name for channel
        minVal: -10.0, // optional, default -10.0
        maxVal: 10.0 // optional, default 10.0
    }],

    sampleTiming: { // timing for individual samples
        terminal: "OnboardClock", // optional, default is 'OnboardClock'
        rate: 10000.0, // sample rate in Hz
        triggerSlope: "falling", // optional, default rising
        sampleMode: "continuous", // optional, default finite
        samplesPerChannel: 10 // max number of samples to read per round
    },

    startTiming: { // optional timing for when to start the task
        type: "digital", // analog, or digital
        terminal: "PFI0",
        triggerSlope: "falling" // optional, default rising
    }
});
```

To do a finite sample of a single channel using an external clock, and an external analog start trigger including hysteresis.

```JavaScript
var task = new daqmx.AIVoltageTask({

    name: "myAIVoltageTask",

    device: "Dev1",

    channels: [{
        terminal: "ai0",
        minVal: -10.0,
        maxVal: 10.0
    }],

    sampleTiming: {
        terminal: "PFI7",
        rate: 10000.0,
        triggerSlope: "rising",
        sampleMode: "finite",
        samplesPerChannel: 10
    },

    startTiming: {
        type: "analog",
        terminal: "APFI0",
        triggerSlope: "rising", // optional, default rising
        triggerLevel: 1.0, // analog trigger only in volts
        hysteresis: 1.0 // optional for analog trigger only, default 0.0
    }
});
```
