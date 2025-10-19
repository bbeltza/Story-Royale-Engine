#include <SDL_ttf.h>
#include <SDL_image.h>
#include "internal.h"

#include "Sys.h"

void __initialize_engine()
{
#pragma region SDLAPI
    SDL_LogSetPriority(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_DEBUG); // Enable SDL error logging
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0"); // Don't interpret touch events as mouse events

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
#pragma endregion


}

void __run_engine()
{

}

void __end_engine()
{
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}