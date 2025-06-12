#pragma once
#include <SDL.hpp>

namespace Engine
{
    namespace Window
    {
        extern SDL_Window* sdl_Window;
        extern SDL_Renderer* sdl_Renderer;

        void render();
    }
}