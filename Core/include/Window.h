#pragma once
#include <SDL.hpp>

#define ENTITY_DRAWCENTERDBG 1

namespace Engine
{
    namespace Window
    {
        extern SDL_Window* sdl_Window;
        extern SDL_Renderer* sdl_Renderer;

        void getScreenCenter(unsigned int* x, unsigned int* y);

        void render();
    }
}