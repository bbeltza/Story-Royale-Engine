#include <iostream>

#include "Window.h"
#include "ECS.h"

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
    // Updating the viewport rect
    SDL_GetWindowSizeInPixels(sdl_Window, &Viewport.w, &Viewport.h);
    Viewport.x = Viewport.w / 2;
    Viewport.y = Viewport.h / 2;

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
        Game::currentWorld->render();
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

void Game::World::render()
{
    Engine::Window::getScreenCenter(this->center, this->center + 1);
    for (Entity* entity : this->m_Entities)
    {
        entity->preRender();
        entity->render();
        entity->postRender();
    }

}

void Game::Entity::render()
{
    unsigned int& cx = m_world->center[0], cy = m_world->center[1];
    struct World::v2 rPt = m_world->worldToScreenSpace(this->x, this->y);

#ifdef _DEBUG
    if (this->m_Components.empty())
    {

        SDL_SetRenderDrawColor(Engine::Window::sdl_Renderer, 255, 64, 0, 255);
        SDL_RenderDrawLineF(Engine::Window::sdl_Renderer, rPt.x - 5, rPt.y, rPt.x + 5, rPt.y);
        SDL_RenderDrawLineF(Engine::Window::sdl_Renderer, rPt.x, rPt.y - 5, rPt.x, rPt.y + 5);
    }
    else
    {
#endif

        for (Component* component : this->m_Components)
            component->render(rPt.x, rPt.y);
#ifdef _DEBUG
    }
#endif
}

void Game::Component::render(int x, int y)
{
    if (this->m_type == SHAPE)
    {
        Color4& c = ShapeComponent.Color;
        SDL_Rect r{ x + ShapeComponent.xOffset, y + ShapeComponent.yOffset, ShapeComponent.Width, ShapeComponent.Height };
        r.x -= r.w / 2;
        r.y -= r.h / 2;

        SDL_SetRenderDrawColor(Engine::Window::sdl_Renderer, c.r, c.b, c.g, c.a);
        SDL_RenderFillRect(Engine::Window::sdl_Renderer, &r);
    }

}

struct Game::World::v2 Game::World::screenToWorldSpace(int x, int y)
{
    unsigned int& cx = this->center[0], cy = this->center[1];
    return { x - (int)cx + (int)CurrentCamera.x, y - (int)cy + (int)CurrentCamera.y };
}

struct Game::World::v2 Game::World::worldToScreenSpace(float x, float y)
{
    unsigned int& cx = this->center[0], cy = this->center[1];
    return { (int)x + (int)cx - (int)CurrentCamera.x, (int)y + (int)cy - (int)CurrentCamera.y };
}