#include <standard.h>

#include "Window.h"
#include "ECS.h"
#include "GuiLayer.h"
#include "Components.h"

static SDL_Rect Viewport;

void Engine::Window::getScreenCenter(unsigned int* x, unsigned int* y)
{
    if (x)
        *x = Viewport.x;
    if (y)
        *y = Viewport.y;
}

void Engine::Window::render()
{
    // Render current world
    //// Aliases for the background and the foreground (so that typing Game::currentWorld wouldn't be necessary)
    Color3& bg = Game::currentWorld->Background;
    Color4& fg = Game::currentWorld->Foreground;
    Color4& uifg = Game::currentGuiLayer->Foreground;

    //// Clearing the screen with the background color
    SDL_SetRenderDrawColor(sdl_Renderer, bg.r, bg.g, bg.b, 255);
    SDL_RenderClear(sdl_Renderer);

    //// Drawing all the entities (doesn't run if the foreground is full opaque)
    if (fg.a < 255)
    {
        Game::currentWorld->render();
    }

    //// Finally, filling the foreground (doesn't run if the foreground is invisible)
    //printf("%u, %u, %u, %u / %u, %u, %u\n", fg.r, fg.g, fg.b, fg.a, bg.r, bg.g, bg.b);
    if (fg.a)
    {
        SDL_SetRenderDrawColor(sdl_Renderer, fg.r, fg.g, fg.b, fg.a);
        SDL_RenderFillRect(sdl_Renderer, NULL);
    }

    // Drawing the Gui layer

    if (uifg.a < 255)
    {
        Game::currentGuiLayer->_render();
    }
    if (uifg.a)
    {
        SDL_SetRenderDrawColor(sdl_Renderer, uifg.r, uifg.g, uifg.b, uifg.a);
        SDL_RenderFillRect(sdl_Renderer, NULL);
    }


    // Present the screen
    SDL_RenderPresent(sdl_Renderer);
}

void Game::Components::Shape::render(int x, int y)
{
    if (!(this->flags & FLAG_ShapeFlags::VISIBLE)) return;

    SDL_SetRenderDrawColor(Engine::Window::sdl_Renderer, Color.r, Color.b, Color.g, Color.a);

    const float cx = x + xOffset;
    const float cy = y + yOffset;

    if (shape == CIRCLE)
    {
        SDL_RenderFillCircle(Engine::Window::sdl_Renderer, cx, cy, Width / 2);
    }
    else
    {
        m_renderRect.w = Width;
        m_renderRect.h = Height;
        m_renderRect.x = cx - m_renderRect.w / 2;
        m_renderRect.y = cy - m_renderRect.h / 2;

        SDL_RenderFillRectF(Engine::Window::sdl_Renderer, &m_renderRect);
    }
}

Vector2f Game::World::screenToWorldSpace(int x, int y)
{
    unsigned int& cx = this->center[0], cy = this->center[1];
    return { x - (int)cx + CurrentCamera.x, y - (int)cy + CurrentCamera.y };
}

Vector2i Game::World::worldToScreenSpace(float x, float y)
{
    unsigned int& cx = this->center[0], cy = this->center[1];
    return { (int)x + (int)cx - (int)CurrentCamera.x, (int)y + (int)cy - (int)CurrentCamera.y };
}

void Engine::Window::processGui()
{
    // Updating the viewport rect
    SDL_GetWindowSizeInPixels(sdl_Window, &Viewport.w, &Viewport.h);
    Viewport.x = Viewport.w / 2;
    Viewport.y = Viewport.h / 2;

    Game::currentGuiLayer->p_absolute.w = (float)Viewport.w; // Apparently, uhh this is modifying the x value instead of the width value
    Game::currentGuiLayer->p_absolute.h = (float)Viewport.h;

    Game::currentGuiLayer->_processchildren();
}