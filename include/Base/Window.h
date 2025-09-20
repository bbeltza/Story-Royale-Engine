#pragma once
#include <SDL.hpp>
#include "EngineBase.h"

class WindowClass
{
    ENGINE_BASE
    friend class GuiContainer;
public:
    void setTargetFPS(unsigned short fps);
    void toggleFullscreen();

    inline void show() { SDL_ShowWindow(sdl_window); }
    inline void hide() { SDL_HideWindow(sdl_window); }

    inline void focus() { SDL_RaiseWindow(sdl_window); }
public:
    inline bool isFullScreen() { return fullscreen; }
    inline bool isHidden() { return SDL_GetWindowFlags(sdl_window) & SDL_WINDOW_HIDDEN; }
private:

    WindowClass(): sdl_window(nullptr) {}

    SDL_Window* sdl_window;
    SDL_Event sdl_event;

    bool fullscreen = false;
};
