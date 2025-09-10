#include <standard.h>

#include "Engine.h"
#include "System.h"

#include "Game/World.h"
#include "Game/Component.h"
#include "Game/GuiLayer.h"

#include "GameSettings.h"

#include "Classes/Timer.h"
#include "Classes/Tween.h"

#define WCENTERED SDL_WINDOWPOS_CENTERED

#define DEF_BASE(Base) Base.m_Engine = this;

static std::vector<GameInstance*> destroyQueue;

static void queueDestroyingInstances();

static std::chrono::duration<double> targetFrameTime;
const static std::chrono::duration<double> zero = std::chrono::duration<float>::zero();

EngineClass* Engine = nullptr;

EngineClass::EngineClass()
{
    Engine = this;

    DEF_BASE(Window)
    DEF_BASE(Input)
    DEF_BASE(DrawingContext)
    DEF_BASE(AudioDevice)

    SDL_LogSetPriority(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_DEBUG);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    //Mix_Init(MIX_INIT_OGG);

    //Mix_OpenAudio(32000, AUDIO_S16, 2, 2048);

    Game::Initialize();

    if (GameSettings::ScalingResolution)
    {
        Vector2i s = GameSettings::StartResolution / GameSettings::ScalingResolution;
        DrawingContext.scale = s.getMin();
    }

    Vector2u start_res = GameSettings::StartResolution ? GameSettings::StartResolution : GameSettings::ScalingResolution * 2;
    Flags32 windowFlags;
    Flags32 rendererFlags;

    if (GameSettings::WindowOptions.Resizable)
        windowFlags.ToggleOn(SDL_WINDOW_RESIZABLE);
    if (GameSettings::WindowOptions.VSync)
        rendererFlags.ToggleOn(SDL_RENDERER_PRESENTVSYNC);

    Window.sdl_window = SDL_CreateWindow(
        GameSettings::Title,
        WCENTERED,
        WCENTERED,
        start_res.X,
        start_res.Y,
        windowFlags
        );

    DrawingContext.sdl_renderer = SDL_CreateRenderer(Window.sdl_window, -1, rendererFlags);
    SDL_SetRenderDrawBlendMode(DrawingContext.sdl_renderer, SDL_BLENDMODE_BLEND);
    
    DrawingContext.sdl_rectTexture = SDL_CreateTexture(DrawingContext.sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, 1, 1);
    {
        uint64_t WHITE = UINT64_MAX;
        SDL_UpdateTexture(DrawingContext.sdl_rectTexture, NULL, &WHITE, 4);
    }
}

EngineClass::~EngineClass()
{
    if (Game::World::m_Current)
        delete Game::World::m_Current;
    if (Game::GuiLayer::m_Current)
        delete Game::GuiLayer::m_Current;

    queueDestroyingInstances();

    //Mix_CloseAudio();
    //Mix_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    Engine = nullptr;

    printf("Engine destructed...\n");
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
    

    SDL_MaximizeWindow(sdl_window);
    SDL_SetWindowFullscreen(sdl_window, fullscreen * SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void EngineClass::Run()
{
    if (m_wasRun) return;

    m_wasRun = 1;

    Window.setTargetFPS(GameSettings::TargetFPS);

    Timer::s_last_frame_time = Timer::s_global_clock.now();

    while (pollWindowEvents())
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
}

bool System::m_exiting = false;
bool EngineClass::pollWindowEvents()
{
    if (System::isExiting()) return 0;

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