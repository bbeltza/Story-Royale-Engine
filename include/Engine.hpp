#pragma once

//
#include <fmt/format.h>
#include <SDL.hpp>

#include "Base/Window.hpp"
#include "Base/Input.hpp"
#include "Base/DrawingContext.hpp"
#include "Base/AudioDevice.hpp"
#include "Base/ThreadPool.hpp"

class EngineClass
{
public:
    // Engine namespaces
    ThrdPool ThreadPool{this};
    WindowClass Window{this};
    InputClass Input{this};
    DrawingDevice DrawingContext{this};
    AudDevice AudioDevice{this};

    Signal OnUpdate{this};
    Signal BeforeRender{this, false};
    Signal AfterRender{this, false};

    // Get the current frame count
    // @returns The frame count starting from 0, or -1 if the application hasn't been run yet
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

    SDL_Thread* m_entryThread;

    long long m_frame = -1;
};

extern EngineClass* const & Engine;

#include "GameEntry.h"