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
    ThrdPool ThreadPool;
    WindowClass Window;
    InputClass Input;
    DrawingDevice DrawingContext;
    AudDevice AudioDevice;

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

    void loop();
    void run_multithreaded();
    void Run();

    static int eventfilter(EngineClass *engine, SDL_Event *ev);

    bool pollWindowEvents();

    // Private members

    bool m_wasRun = false;

    bool n_dtrc = false;

    SDL_Thread* m_entryThread;
};

extern EngineClass* Engine;

#include "GameEntry.h"