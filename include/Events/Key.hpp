#pragma once
#include <SDL_events.h>

struct Key
{
    Key() = default;
    Key(const SDL_KeyboardEvent* ev):
        pressed(ev->state),
        keyCode((SDL_KeyCode)ev->keysym.sym),
        scanCode((SDL_Scancode)ev->keysym.scancode),
        keyModifiers((SDL_Keymod)ev->keysym.mod),
        repeat(ev->repeat)
    {}

    bool pressed;
    SDL_KeyCode keyCode;
    SDL_Scancode scanCode;

    SDL_Keymod keyModifiers;
    Uint8 repeat;
};