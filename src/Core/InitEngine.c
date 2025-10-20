#include <SDL_ttf.h>
#include <SDL_image.h>
#include "../internal.h"

#include "Sys.h"

#include "GameEntry.h"

void __initialize_engine()
{
    SDL_LogSetPriority(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_DEBUG); // Enable SDL error logging
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0"); // Don't interpret touch events as mouse events

    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    const struct _win_settings _win = __setup_window_data();

    __setup_audio_device();

    engine.destroyqueue_mutex = SDL_CreateMutex();
    engine.entry_thread = SDL_CreateThread((SDL_ThreadFunction)Initialize, "Game Entry", NULL);

    __create_window(&_win);
    __setup_renderer(_win.renderflags);
}

void __end_engine()
{
    __destroy_queue();
    SDL_DestroyMutex(engine.destroyqueue_mutex);

    SDL_DetachThread(engine.entry_thread);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}