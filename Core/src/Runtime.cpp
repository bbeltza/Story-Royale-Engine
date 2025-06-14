#include <standard.h>

#include "Engine.h"
#include "Window.h"
#include "Input.h"
#include "ECS.h"
#include "GameSettings.h"

#define WCENTERED SDL_WINDOWPOS_CENTERED

static void EngineRun();
static void EngineClose();
static bool WindowPollEvents();
static SDL_Event sdl_event;

static std::chrono::steady_clock frametime_clock;
static std::chrono::duration<double> targetFrameTime;
const static std::chrono::duration<double> zero = std::chrono::duration<float>::zero();

static char Keys[512];

SDL_Window* Engine::Window::sdl_Window;
SDL_Renderer* Engine::Window::sdl_Renderer;

void Engine::Init()
{
    if (SDL_WasInit(0)) return;

    SDL_Init(SDL_INIT_EVERYTHING);

    GameSettings::onSet();

    Game_SetWorld(EntryWorld)

    Window::sdl_Window = SDL_CreateWindow(GameSettings::Title, WCENTERED, WCENTERED, GameSettings::StartResolution.width, GameSettings::StartResolution.height, SDL_WINDOW_RESIZABLE);
    Window::sdl_Renderer = SDL_CreateRenderer(Window::sdl_Window, -1, 0);
    SDL_SetRenderDrawBlendMode(Window::sdl_Renderer, SDL_BLENDMODE_BLEND);

    EngineRun();
    EngineClose();
}

void Engine::setTargetFPS(unsigned short fps)
{
    targetFrameTime = std::chrono::duration<double>(GameSettings::targetFPS > 0 ? (1.0f / GameSettings::targetFPS) : 0);
}

void EngineRun()
{
    static bool wasRun = 0;
    if (wasRun) return;

    wasRun = 1;

    auto start = frametime_clock.now();
    std::chrono::duration<double> delta;

    Engine::setTargetFPS(GameSettings::targetFPS);

    while (WindowPollEvents())
    {
        delta = frametime_clock.now() - start;
        start = frametime_clock.now();
        float dt = delta.count();

        Game::currentWorld->Update(dt);
        Game::currentWorld->pUpdate(dt);
        Engine::Window::render();

        if (targetFrameTime > zero)
        {
            std::chrono::duration<double> frametime = std::chrono::steady_clock::now() - start;

            auto sleeptime = targetFrameTime - frametime;
            std::this_thread::sleep_for(sleeptime);
        }
        
    }

}

void EngineClose()
{
    if (Game::currentWorld)
        delete Game::currentWorld;
    SDL_Quit();
}

bool WindowPollEvents()
{
    while (SDL_PollEvent(&sdl_event) != 0)
    {
        switch (sdl_event.type)
        {
        case SDL_QUIT:
            return 0;
        case SDL_KEYDOWN:
            if (sdl_event.key.keysym.sym > 512)
                Keys[sdl_event.key.keysym.scancode] |= 3;
            else
            {
                Keys[sdl_event.key.keysym.scancode] |= 1;
                Keys[sdl_event.key.keysym.sym] |= 2;
            }
                
            
            break;
        case SDL_KEYUP:
            if (sdl_event.key.keysym.sym > 512)
                Keys[sdl_event.key.keysym.scancode] &= ~3;
            else
            {
                Keys[sdl_event.key.keysym.scancode] &= ~1;
                Keys[sdl_event.key.keysym.sym] &= ~2;
            }
            break;
        default:
            break;
        }
    }

    return 1;
}

bool Engine::Input::isKeyPressed(SDL_KeyCode keycode)
{
    return Keys[keycode & ~(1 << 30)] & 2;
}

bool Engine::Input::isKeyPressed(SDL_Scancode scancode)
{
    return Keys[scancode] & 1;
}