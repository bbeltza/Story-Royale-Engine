#include <standard.h>

#include "Engine.h"
#include "Window.h"
#include "Input.h"
#include "ECS.h"
#include "GuiLayer.h"
#include "GuiComponents.h"
#include "Components.h"

#include "GameSettings.h"

#define WCENTERED SDL_WINDOWPOS_CENTERED

#define DEF_BASE(Base) Base.m_Engine = this;

static std::vector<GameInstance*> destroyQueue;

static void queueDestroyingInstances();

static std::chrono::steady_clock frametime_clock;
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

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);

    GameSettings::onSet();

    Game::setWorld<EntryWorld>();
    Game::setGuiLayer<EntryGuiLayer>();

    Window.sdl_window = SDL_CreateWindow(
        GameSettings::Title,
        WCENTERED,
        WCENTERED,
        GameSettings::StartResolution.width,
        GameSettings::StartResolution.height,
        SDL_WINDOW_RESIZABLE
        );

    Window.sdl_renderer = SDL_CreateRenderer(Window.sdl_window, -1, 0);
    SDL_SetRenderDrawBlendMode(Window.sdl_renderer, SDL_BLENDMODE_BLEND);
}

EngineClass::~EngineClass()
{
    if (Game::currentWorld)
        delete Game::currentWorld;
    if (Game::currentGuiLayer)
        delete Game::currentGuiLayer;
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void WindowClass::setTargetFPS(unsigned short fps)
{
    targetFrameTime = std::chrono::duration<double>(GameSettings::targetFPS > 0 ? (1.0f / GameSettings::targetFPS) : 0);
}

void EngineClass::Run()
{
    if (m_wasRun) return;

    m_wasRun = 1;

    auto start = frametime_clock.now();
    std::chrono::duration<double> delta;

    Window.setTargetFPS(GameSettings::targetFPS);

    while (pollWindowEvents())
    {
        delta = frametime_clock.now() - start;
        start = frametime_clock.now();
        float dt = delta.count();

        queueDestroyingInstances();

        Input.processEvents();
        Input.queryObjects();

        Game::currentWorld->Update(dt);
        Game::currentWorld->pUpdate(dt);
        
        Window.processGui();
        Window.render();

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
        delete destroyQueue.back();
        destroyQueue.pop_back();
    }
}

void GameInstance::Destroy()
{
    destroyQueue.push_back(this);
}

// Check whatever instance the mouse is actually hovering in



