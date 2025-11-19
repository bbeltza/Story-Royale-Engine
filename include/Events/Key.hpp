#pragma once
#include <SDL_events.h>

struct Key
{
    Key(const SDL_KeyboardEvent* ev):
        pressed(ev->state),
        keyCode((SDL_KeyCode)ev->keysym.sym),
        scanCode((SDL_Scancode)ev->keysym.scancode),
        keyModifiers((SDL_Keymod)ev->keysym.mod),
        repeat(ev->repeat)
    {}

    const bool pressed;
    const SDL_KeyCode keyCode;
    const SDL_Scancode scanCode;

    const SDL_Keymod keyModifiers;
    const Uint8 repeat;
};