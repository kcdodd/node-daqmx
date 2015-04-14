
var daqmx = require('./build/Release/nodedaqmx');


try{

    console.log(daqmx.devices());
/*


    // Continuous samples, internal clock, digital start trigger
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

    // finite samples, external clock, analog start trigger with hysterisis
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
*/

    // geterdone example with two channels
    var task = new daqmx.AIVoltageTask({

        device: "Dev1",

        channels: [{terminal: "ai0:3"}],

        sampleTiming: {
            rate: 10000.0,
            samplesPerChannel: 10
        }
    });


    task.start();

    var x = task.read(); // infinite timeout
    console.log(x);
/*
    // restart same task
    task.stop();
    task.start();

    // do a read asyncronously
    var readAsync = function(callback) {

        process.nextTick(function(){
            // call canRead without argument tests if all samples can be read
            if (task.canRead()) {
                var data = task.read(0.0); // no timeout because I know its ready

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
*/





}catch(e){
    console.log(e);
}
