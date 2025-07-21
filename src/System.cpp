#include <SDL.hpp>
#include <standard.h>
#include "System.h"

void System::Sleep(float Duration)
{
    std::this_thread::sleep_for(std::chrono::duration<float>(Duration));
}

const char* System::GetSDLError()
{
    return SDL_GetError();
}