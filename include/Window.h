#pragma once
#include <SDL.hpp>
#include "EngineBase.h"

#define ENTITY_DRAWCENTERDBG 1
#define ENTITY_DBGLINESIZE 3

class WindowClass
{
    ENGINE_BASE
    friend class GuiContainer;
public:
    void setTargetFPS(unsigned short fps);
private:

    WindowClass(): sdl_window(nullptr) {}

    SDL_Window* sdl_window;

    SDL_Event sdl_event;

    SDL_Rect m_viewport;

    bool pollEvents();
    void processViewport();
};