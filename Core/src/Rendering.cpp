#include "Window.h"
#include "ECS.h"

void Engine::Window::render()
{
    // Render current world
    //// Aliases for the background and the foreground (so that typing Game::currentWorld wouldn't be necessary)
    Color3& bg = Game::currentWorld->Background;
    Color4& fg = Game::currentWorld->Foreground;

    //// Clearing the screen with the background color
    SDL_SetRenderDrawColor(sdl_Renderer, bg.r, bg.g, bg.b, 255);
    SDL_RenderClear(sdl_Renderer);
    
    //// Drawing all the entities (doesn't run if the foreground is full opaque)
    if (fg.a < 255)
    {
        Game::currentWorld->Render();
    }

    //// Finally, filling the foreground (doesn't run if the foreground is invisible)
    //printf("%u, %u, %u, %u / %u, %u, %u\n", fg.r, fg.g, fg.b, fg.a, bg.r, bg.g, bg.b);
    if (fg.a)
    {
        SDL_SetRenderDrawColor(sdl_Renderer, fg.r, fg.g, fg.b, fg.a);
        SDL_RenderFillRect(sdl_Renderer, NULL);
    }

    SDL_RenderPresent(sdl_Renderer);
}