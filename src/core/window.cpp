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

sre::vec2i sre::window_getsize(void) {
    return engine.osize;
}
sre::vec2ut sre::window_getviewport(void) {
    return engine.vsize;
}
sre::unit sre::window_getscale(void) {
    return engine.scale;
}
sre::unit sre::window_getscale_ratio(void) {
    return engine.scale_ratio;
}

bool sre::window_set_manualscale(int scale) {
    if (scale <= 0) {
        return false;
    }

    engine.scale = static_cast<sre::unit>(scale);
    engine.scale_ratio = 1/engine.scale;
    return true;
}

bool sre::window_enable_autoscaling(int target_w, int target_h) {
    if (target_w < 0 || target_h < 0 ) {
        // sre::error
        return false;
    }
    if (!target_w && !target_h) { // Use sre::window_disable_autoscaling
        return false;
    }

    // One of the target components can be set to 0, meaning that the autoscaler won't rely on that component to scale the screen
    target_w = target_w ? target_w : INT_MAX;
    target_h = target_h ? target_h : INT_MAX;

    engine.autoscalex = target_w;
    engine.autoscaley = target_h;
    return true;
}
void sre::window_disable_autoscaling(void) {
    engine.autoscalex = 0;
    engine.autoscaley = 0;
}