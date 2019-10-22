{
    "targets": [
        {
            "target_name": "nodedaqmx",
            "sources": ["./src/nodedaqmx.cc", "./src/AIVoltageTask.cc", "./src/error.cc"],
            "include_dirs": ["C:\Program Files (x86)\National Instruments\Shared\ExternalCompilerSupport\C\include", "<!(node -e \"require('nan')\")"],
            "libraries": ["C:\Program Files (x86)\National Instruments\Shared\ExternalCompilerSupport\C\lib64\msvc\NIDAQmx.lib"]
        }
    ]
}
