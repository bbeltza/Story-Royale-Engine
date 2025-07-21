#pragma once

#include <fmt/format.h>
#include "SDL.hpp"

#include "Window.h"
#include "Input.h"
#include "DrawingContext.h"
#include "AudioDevice.h"

class EngineClass
{
public:


    // Engine namespaces
    WindowClass Window;
    InputClass Input;
    DrawingDevice DrawingContext;
    AudioDevice Audio;

    Signal OnUpdate;
    Signal BeforeRender;
    Signal AfterRender;
private:
    // Friend int main so that the entry point runs void Run()
    friend int main();
    // And friend throw_error(int) to handle errors
    friend void throw_error(int);

    // Constructors
    EngineClass();
    ~EngineClass();

    // Private use functions
    void Run();

    bool pollWindowEvents();

    // Private members
    bool m_wasRun = 0;
};

extern EngineClass* Engine;

