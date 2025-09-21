#pragma once
#include <SDL.hpp>
#include "EngineBase.h"

#include "Datatypes/Vector.h"
#include "Datatypes/Flags.h"

class WindowClass
{
    ENGINE_BASE
    friend class DrawingDevice;
    friend class GuiContainer;
public:
    void setTargetFPS(unsigned short fps);
    void toggleFullscreen();

    inline void show() { SDL_ShowWindow(sdl_window); }
    inline void hide() { SDL_HideWindow(sdl_window); }

    inline void focus() { SDL_RaiseWindow(sdl_window); }

    void setIcon(const char* path);
public:
    inline bool isFullScreen() { return fullscreen; }
    inline bool isHidden() { return SDL_GetWindowFlags(sdl_window) & SDL_WINDOW_HIDDEN; }
private:
    WindowClass(EngineClass*);

    SDL_Window* sdl_window = nullptr;
    SDL_Event sdl_event;

    bool fullscreen = false;

    void Setup();

    Flags32 start_flags;
    Vector2f start_res;
};
