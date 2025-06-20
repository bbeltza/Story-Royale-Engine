#pragma once
#include <SDL.hpp>

#include "Datatypes.h"

enum ENUM_MouseButton: Uint8
{
    mbLeft = SDL_BUTTON_LEFT,
    mbMiddle = SDL_BUTTON_MIDDLE,
    mbRight = SDL_BUTTON_RIGHT,
    mbX1 = SDL_BUTTON_X1,
    mbX2 = SDL_BUTTON_X2
};

namespace Engine
{
    namespace Input
    {
        bool isKeyPressed(SDL_KeyCode keycode);
        bool isKeyPressed(SDL_Scancode scancode);

        bool isMouseButtonPressed(ENUM_MouseButton button);
        Vector2i getMouseScreenPosition();
        Vector2f getMouseWorldPosition();
    }
}