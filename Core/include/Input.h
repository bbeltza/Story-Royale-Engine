#pragma once
#include <SDL.hpp>

namespace Engine
{
    namespace Input
    {
        bool isKeyPressed(SDL_KeyCode keycode);
        bool isKeyPressed(SDL_Scancode scancode);
    }
}