#include "../internal.h"

#include <Core/Window.hpp>
#include <Base/Image.hpp>

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

void sre::window_setresizable(bool resizable)
{
    SDL_SetWindowResizable(engine.sdl_windowhndl, static_cast<SDL_bool>(resizable));
}

static inline bool flashwindowop(SDL_FlashOperation op) { return SDL_FlashWindow(engine.sdl_windowhndl, op) == 0; }
bool sre::window_flash() { return flashwindowop(SDL_FLASH_UNTIL_FOCUSED); }
bool sre::window_quickflash() { return flashwindowop(SDL_FLASH_BRIEFLY); }
bool sre::window_cancelflash() { return flashwindowop(SDL_FLASH_CANCEL); }

bool sre::window_isfullscreen() { return 0 != (SDL_GetWindowFlags(engine.sdl_windowhndl) & SDL_WINDOW_FULLSCREEN); }
bool sre::window_ishidden() { return 0 != (SDL_GetWindowFlags(engine.sdl_windowhndl) & SDL_WINDOW_HIDDEN); }

// Deferred functions (they require to be run on the main thread otherwise it's undefined behavior)
#include <Core/Defer.h>

void sre::window_setsize(int w, int h)
{
    defer([](int w, int h) { SDL_SetWindowSize(engine.sdl_windowhndl, w, h); }, w, h);
}