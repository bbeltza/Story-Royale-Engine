#pragma once

//
#include <fmt/format.h>
#include "SDL.hpp"

#include "Base/Window.h"
#include "Base/Input.h"
#include "Base/DrawingContext.h"
#include "Base/AudioDevice.h"

class EngineClass
{
public:
    // Engine namespaces
    WindowClass Window;
    InputClass Input;
    DrawingDevice DrawingContext;
    AudioDevice AudioDevice;

    Signal OnUpdate;
    Signal BeforeRender;
    Signal AfterRender;
private:
    // Friend int main so that the entry point runs void Run()
    friend int main();

    // Constructors
    EngineClass();
    ~EngineClass();

    // Private use functions

    void Run();

    bool pollWindowEvents();

    // Private members

    bool m_wasRun = false;

    bool n_dtrc = false;
};

extern EngineClass* Engine;

#define init extern "C" void Initialize()

namespace Game
{
    // The method to be defined by the game using the engine
    // EVERY game should have this defined, even if it's empty
    // This is the entry point for the game, it's called after initializing the libraries, but before creating the window
    // You can change the game settings here (Untill a json implementation is goind to be added)
    init;
}

init;

#undef init