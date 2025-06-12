#include <iostream>

#include "Engine.h"
#include "Window.h"
#include "ECS.h"
#include "GameSettings.h"
#include "SDL.hpp"

#define WCENTERED SDL_WINDOWPOS_CENTERED

static void EngineRun();
static void EngineClose();
static bool WindowPollEvents();
static SDL_Event sdl_event;

SDL_Window* Engine::Window::sdl_Window;
SDL_Renderer* Engine::Window::sdl_Renderer;

void Engine::Init()
{
    if (SDL_WasInit(0)) return;

    SDL_Init(SDL_INIT_EVERYTHING);

    GameSettings::onSet();

    Game_SetWorld(EntryWorld)

    Window::sdl_Window = SDL_CreateWindow(GameSettings::Title, WCENTERED, WCENTERED, GameSettings::StartResolution::width, GameSettings::StartResolution::height, 0);
    Window::sdl_Renderer = SDL_CreateRenderer(Window::sdl_Window, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawBlendMode(Window::sdl_Renderer, SDL_BLENDMODE_BLEND);

    EngineRun();
    EngineClose();
}




void EngineRun()
{
    static bool wasRun = 0;
    if (wasRun) return;

    wasRun = 1;

    std::cout << "!\n";

    while (WindowPollEvents())
    {
        Engine::Window::render();
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
        default:
            break;
        }

    }

    return 1;
}

