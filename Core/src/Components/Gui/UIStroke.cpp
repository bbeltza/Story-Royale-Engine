#include "Window.h"
#include "GuiComponents.h"

void Game::GuiComponents::UIStroke::render()
{
    SDL_SetRenderDrawColor(Engine::Window::sdl_Renderer, color.r, color.g, color.b, color.a);

    for (int i = 0; i < size; i++)
    {
        SDL_FRect r = *getParentAbs();
        r.h -= i * 2;
        r.w -= i * 2;
        r.x += i;
        r.y += i;

        SDL_RenderDrawRectF(Engine::Window::sdl_Renderer, &r);
    }
}