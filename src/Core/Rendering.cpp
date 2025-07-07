#include <standard.h>

#include "ColorFunctions.h"
#include "Engine.h"
#include "ECS.h"
#include "GuiLayer.h"
#include "Components.h"
#include "GuiComponents.h"

void WindowClass::getScreenCenter(unsigned int* x, unsigned int* y)
{
    if (x)
        *x = m_viewport.x;
    if (y)
        *y = m_viewport.y;
}

void DrawingDevice::render()
{
    m_switchLock();
    tr();

    // Render current world
    if (Game::currentWorld) renderCurrentWorld();
    else
    {
        SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 0);
        SDL_RenderClear(sdl_renderer);
    }

    m_Engine->BeforeRender.Fire(0);

    // Drawing the Gui layer
    if (Game::currentGuiLayer) renderCurrentUI();
    
    m_Engine->AfterRender.Fire(0);

    // Present the screen
    m_switchLock();
    SDL_RenderPresent(sdl_renderer);
}

void DrawingDevice::renderCurrentWorld()
{    
    //// Aliases for the background and the foreground (so that typing Game::currentWorld wouldn't be necessary)
    Color3& bg = Game::currentWorld->Background;
    Color4& fg = Game::currentWorld->Foreground;

    //// Clearing the screen with the background color
    SDL_SetRenderDrawColor(sdl_renderer, bg.r, bg.g, bg.b, 255);
    SDL_RenderClear(sdl_renderer);

    //// Drawing all the entities (doesn't run if the foreground is full opaque)
    if (fg.a < 255)
    {
        Game::currentWorld->render();
    }

    //// Finally, filling the foreground (doesn't run if the foreground is invisible)
    //printf("%u, %u, %u, %u / %u, %u, %u\n", fg.r, fg.g, fg.b, fg.a, bg.r, bg.g, bg.b);
    if (fg.a)
    {
        SDL_SetRenderDrawColor(sdl_renderer, fg.r, fg.g, fg.b, fg.a);
        SDL_RenderFillRect(sdl_renderer, NULL);
    }
}

void DrawingDevice::renderCurrentUI()
{
    Color4& uifg = Game::currentGuiLayer->Foreground;

    if (uifg.a < 255)
    {
        Game::currentGuiLayer->preRender();
        Game::currentGuiLayer->_render();
        Game::currentGuiLayer->postRender();
    }
    if (uifg.a)
    {
        SDL_SetRenderDrawColor(sdl_renderer, uifg.r, uifg.g, uifg.b, uifg.a);
        SDL_RenderFillRect(sdl_renderer, NULL);
    }
}

//

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

void WindowClass::processViewport()
{
    // Updating the viewport rect
    SDL_GetWindowSizeInPixels(sdl_window, &m_viewport.w, &m_viewport.h);
    m_viewport.x = m_viewport.w / 2;
    m_viewport.y = m_viewport.h / 2;

    if (!Game::currentGuiLayer) return;    
    Game::currentGuiLayer->p_absolute.w = (float)m_viewport.w;
    Game::currentGuiLayer->p_absolute.h = (float)m_viewport.h;

    Game::currentGuiLayer->_processchildren();
}