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

    // Get the number of frames that the engine has rendered
    inline long long runtime_frame() const {return m_frame;}
private:
    // Friend int main so that the entry point runs void Run()
    friend int main();

    // Constructors
    EngineClass();
    ~EngineClass();

    // Private use functions

    void loop();
    void Run();

    static int eventfilter(EngineClass *engine, SDL_Event *ev);

    bool pollWindowEvents();

    // Private members

    bool m_wasRun = false;

    bool n_dtrc = false;

    SDL_Thread* m_entryThread;

    long long m_frame = -1;
};

extern EngineClass* Engine;

#include "GameEntry.h"