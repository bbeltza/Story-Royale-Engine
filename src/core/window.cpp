#include "../internal.h"

#include <Core/Window.hpp>
#include <Base/Image.hpp>

bool sre::window_seticon(const sre::Image& img)
{
    SDL_Surface* surface = static_cast<SDL_Surface*>(img.handle());
    if (!surface) return false;

    SDL_ClearError();
    SDL_SetWindowIcon(engine.sdl_windowhndl, surface);
    if (SDL_GetError()[0])
        return false;
    
    return true;
}

bool sre::window_togglefullscreen()
{
    bool fullscreen = !window_isfullscreen();

    return SDL_SetWindowFullscreen(engine.sdl_windowhndl, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) == 0;
}

void sre::window_show() { return SDL_ShowWindow(engine.sdl_windowhndl); }
void sre::window_hide() { return SDL_HideWindow(engine.sdl_windowhndl); }
void sre::window_focus() { return SDL_RaiseWindow(engine.sdl_windowhndl); }

void sre::window_minimize() { return SDL_MinimizeWindow(engine.sdl_windowhndl); }
void sre::window_maximize() { return SDL_MaximizeWindow(engine.sdl_windowhndl); }

void sre::window_setsize(int w, int h) { return SDL_SetWindowSize(engine.sdl_windowhndl, w, h); }
void sre::window_setresizable(bool resizable) { return SDL_SetWindowResizable(engine.sdl_windowhndl, static_cast<SDL_bool>(resizable)); }

static inline bool flashwindowop(SDL_FlashOperation op) { return SDL_FlashWindow(engine.sdl_windowhndl, op) == 0; }
bool sre::window_flash() { return flashwindowop(SDL_FLASH_UNTIL_FOCUSED); }
bool sre::window_quickflash() { return flashwindowop(SDL_FLASH_BRIEFLY); }
bool sre::window_cancelflash() { return flashwindowop(SDL_FLASH_CANCEL); }

bool sre::window_isfullscreen() { return SDL_GetWindowFlags(engine.sdl_windowhndl) & SDL_WINDOW_FULLSCREEN; }
bool sre::window_ishidden() { return SDL_GetWindowFlags(engine.sdl_windowhndl) & SDL_WINDOW_HIDDEN; }
