#include "Sys.h"
#include "SDL.h"

void sysleeps(float s)
{
    SDL_Delay((Uint32)(s * 1000));
}

void sysleepms(uint32_t ms)
{
    SDL_Delay(ms);
}