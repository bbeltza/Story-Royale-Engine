#pragma once
#include <SDL.hpp>

struct Key
{
    Key(bool p, int kc, int sc, int km, Uint8 r):
        pressed(p),
        keyCode((SDL_KeyCode)kc),
        scanCode((SDL_Scancode)sc),
        keyModifiers((SDL_Keymod)km),
        repeat(r)
    {}

    bool pressed;
    SDL_KeyCode keyCode;
    SDL_Scancode scanCode;

    SDL_Keymod keyModifiers;
    Uint8 repeat;
};