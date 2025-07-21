#include <standard.h>

#include "Engine.h"
#include "Window.h"
#include "Input.h"
#include "ECS.h"
#include "GuiLayer.h"
#include "GuiComponents.h"
#include "Components.h"

#include "GameSettings.h"

#include "Timer.h"
#include "Tween.h"

#define WCENTERED SDL_WINDOWPOS_CENTERED

#define DEF_BASE(Base) Base.m_Engine = this;

static std::vector<GameInstance*> destroyQueue;

static void queueDestroyingInstances();

static std::chrono::duration<double> targetFrameTime;
const static std::chrono::duration<double> zero = std::chrono::duration<float>::zero();

EngineClass* Engine = nullptr;

EngineClass::EngineClass()
{
    // Useful in case of restarting the whole engine by just constructing another one
    if (Engine) delete Engine;
    Engine = this;

    DEF_BASE(Window)
    DEF_BASE(Input)
    DEF_BASE(DrawingContext)
    DEF_BASE(Audio)

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    Mix_Init(MIX_INIT_OGG);

    Mix_OpenAudio(32000, AUDIO_S16, 2, 2048);

    Game::Initialize();

    if (GameSettings::ScalingResolution)
    {
        Vector2i s = GameSettings::StartResolution / GameSettings::ScalingResolution;
        DrawingContext.scale = s.getMin();
    }

    Window.sdl_window = SDL_CreateWindow(
        GameSettings::Title,
        WCENTERED,
        WCENTERED,
        GameSettings::StartResolution.X,
        GameSettings::StartResolution.Y,
        SDL_WINDOW_RESIZABLE
        );

    DrawingContext.sdl_renderer = SDL_CreateRenderer(Window.sdl_window, -1, 0);
    SDL_SetRenderDrawBlendMode(DrawingContext.sdl_renderer, SDL_BLENDMODE_BLEND);
    
    DrawingContext.sdl_rectTexture = SDL_CreateTexture(DrawingContext.sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, 1, 1);
    {
        uint64_t WHITE = UINT64_MAX;
        SDL_UpdateTexture(DrawingContext.sdl_rectTexture, NULL, &WHITE, 4);
    }
}

EngineClass::~EngineClass()
{
    Game::Component::ClearComponents();

    if (Game::currentWorld)
        delete Game::currentWorld;
    if (Game::currentGuiLayer)
        delete Game::currentGuiLayer;
    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void WindowClass::setTargetFPS(unsigned short fps)
{
    targetFrameTime = std::chrono::duration<double>(GameSettings::targetFPS > 0 ? (1.0f / GameSettings::targetFPS) : 0);
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

    Window.setTargetFPS(GameSettings::targetFPS);

    while (pollWindowEvents())
    {
        auto start = std::chrono::steady_clock::now();
        float dt = Timer::global_update();
        Tween::global_update(dt);

        queueDestroyingInstances();

        Input.processEvents();
        Input.queryObjects();

        if (Game::currentWorld)
        {
            Game::currentWorld->Update(dt);
            Game::currentWorld->pUpdate(dt);
        }
        
        DrawingContext.processViewport();
        DrawingContext.render();

        if (Game::currentGuiLayer)
            Game::currentGuiLayer->_callUpdate(dt);

        if (targetFrameTime > zero)
        {
            std::chrono::duration<double> frametime = std::chrono::steady_clock::now() - start;

            auto sleeptime = targetFrameTime - frametime;
            std::this_thread::sleep_for(sleeptime);
        }
    }

}

bool EngineClass::pollWindowEvents()
{
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
        if (destroyQueue.back() == Game::currentGuiLayer) Game::currentGuiLayer = nullptr;
        if (destroyQueue.back() == Game::currentWorld) Game::currentWorld = nullptr;
        delete destroyQueue.back();
        destroyQueue.pop_back();
    }
}

void GameInstance::Destroy()
{
    destroyQueue.push_back(this);
}



