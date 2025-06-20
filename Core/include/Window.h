#pragma once
#include <SDL.hpp>

#define ENTITY_DRAWCENTERDBG 1
#define ENTITY_DBGLINESIZE 3

namespace Engine
{
    namespace Window
    {
        extern SDL_Window* sdl_Window;
        extern SDL_Renderer* sdl_Renderer;

        void getScreenCenter(unsigned int* x, unsigned int* y);

        void render();
        void processGui();
    }
}