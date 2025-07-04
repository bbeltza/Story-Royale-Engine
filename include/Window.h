#pragma once
#include <SDL.hpp>
#include "EngineBase.h"

#define ENTITY_DRAWCENTERDBG 1
#define ENTITY_DBGLINESIZE 3

class WindowClass
{
    ENGINE_BASE
public:
    void getScreenCenter(uint32_t* x, uint32_t* y);
    void setTargetFPS(unsigned short fps);
private:

    WindowClass() {}

    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;

    SDL_Event sdl_event;

    SDL_Rect m_viewport;

    bool pollEvents();
    void processGui();
    void render();
    void target_render();
};