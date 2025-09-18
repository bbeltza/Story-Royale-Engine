#pragma once

//
#include <fmt/format.h>
#include "SDL.hpp"

#include "Base/Window.h"
#include "Base/Input.h"
#include "Base/DrawingContext.h"
#include "Base/AudioDevice.h"
#include "Base/ThreadPool.h"

class EngineClass
{
public:
    // Engine namespaces
    WindowClass Window;
    InputClass Input;
    DrawingDevice DrawingContext;
    AudioDevice AudioDevice;
    ThreadPool ThreadPool;

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

    SDL_Thread* m_entryThread;
};

extern EngineClass* Engine;

#include "GameEntry.h"