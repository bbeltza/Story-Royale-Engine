#include <standard.h>

#include "Engine.h"
#include "System.h"

#include "Game/World.h"
#include "Game/Component.h"
#include "Game/GuiLayer.h"

#include "GameSettings.h"

#include "Classes/Timer.h"
#include "Classes/Tween.h"

static std::vector<GameInstance *> destroyQueue;

static void queueDestroyingInstances();

static std::chrono::duration<double> targetFrameTime;
const static std::chrono::duration<double> zero = std::chrono::duration<float>::zero();

EngineClass *Engine = nullptr;

EngineClass::EngineClass() : BeforeRender(false), AfterRender(false),
                             ThreadPool(this),
                             Window(this),
                             Input(this),
                             DrawingContext(this),
                             AudioDevice(this)

{
    Engine = this;

    SDL_LogSetPriority(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_DEBUG);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    m_entryThread = SDL_CreateThread((SDL_ThreadFunction)Game::Initialize, "Entry", NULL);

    Window.Setup();
    DrawingContext.Setup();
}

EngineClass::~EngineClass()
{
    SDL_DetachThread(m_entryThread);
    ThreadPool.detach_threads();

    if (Game::World::m_Current)
        delete Game::World::m_Current;
    if (Game::GuiLayer::m_Current)
        delete Game::GuiLayer::m_Current;

    queueDestroyingInstances();

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    Engine = nullptr;
}

void WindowClass::setTargetFPS(unsigned short fps)
{
    targetFrameTime = std::chrono::duration<TimeStamp>(GameSettings::TargetFPS > 0 ? (1.0f / GameSettings::TargetFPS) : 0);
}

void WindowClass::toggleFullscreen()
{
    static int oW, oH; // Old width and height for the window
    fullscreen = !fullscreen;
    if (fullscreen)
    {
        SDL_DisplayMode d;
        SDL_GetDesktopDisplayMode(0, &d);
        SDL_SetWindowDisplayMode(sdl_window, &d);
    }

    if (fullscreen)
        SDL_GetWindowSize(sdl_window, &oW, &oH);

    SDL_SetWindowFullscreen(sdl_window, fullscreen * SDL_WINDOW_FULLSCREEN_DESKTOP);
}

int EngineClass::eventfilter(EngineClass *engine, SDL_Event *ev)
{
    #ifdef _WIN32
    if (ev->type == SDL_WINDOWEVENT && ev->window.event == SDL_WINDOWEVENT_EXPOSED)
    {
        engine->loop();
    }
    #endif

    return 1;
}

void EngineClass::Run()
{
    if (m_wasRun)
        return;
    m_wasRun = 1;

    Window.setTargetFPS(GameSettings::TargetFPS);

    SDL_SetEventFilter((SDL_EventFilter)eventfilter, this);

    Timer::s_last_frame_time = Timer::s_global_clock.now();
    while (pollWindowEvents())
    {
        loop();
    }
}

void EngineClass::run_multithreaded()
{
    while (1)
    {
        
    }
}

void EngineClass::loop()
{
    auto start = Timer::s_global_clock.now();
    TimeStamp dt = Timer::global_update();
    Tween::global_update(dt);

    Input.processEvents();

    queueDestroyingInstances();

    Input.queryObjects();

    if (Game::World::m_Current)
    {
        Game::World::m_Current->call_update(dt);
        Game::World::m_Current->call_pupdate(dt);
    }

    if (Game::GuiLayer::m_Current)
        Game::GuiLayer::m_Current->call_update(dt);

    DrawingContext.processViewport();
    DrawingContext.render();

    if (targetFrameTime > zero)
    {
        std::chrono::duration<double> frametime = std::chrono::steady_clock::now() - start;

        auto sleeptime = targetFrameTime - frametime;
        std::this_thread::sleep_for(sleeptime);
    }
}

bool System::m_exiting = false;
bool EngineClass::pollWindowEvents()
{
    if (System::isExiting())
        return 0;

    while (SDL_PollEvent(&Window.sdl_event) != 0)
    {
        switch (Window.sdl_event.type)
        {
        case SDL_QUIT:
            return 0;
        default:
            break;
        }
        Input.processWindowEvents(&Window.sdl_event);
    }

    return 1;
}

void queueDestroyingInstances()
{
    while (!destroyQueue.empty())
    {
        delete destroyQueue.back();
        destroyQueue.pop_back();
    }
}

void GameInstance::Destroy()
{
    destroyQueue.push_back(this);
}
