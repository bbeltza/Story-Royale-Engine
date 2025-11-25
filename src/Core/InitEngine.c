#include <SDL_ttf.h>
#include <SDL_image.h>
#include "../internal.h"

#include "GameEntry.h"

#include <stdlib.h>
#include <stdio.h>

#include <utils/logging.h>

static inline void __setup_engine_data()
{
    engine.input_last_touchid = -1;
    engine.destroyqueue_mutex = SDL_CreateMutex();
    engine.entry_thread = SDL_CreateThread((SDL_ThreadFunction)Initialize, "Game Entry", NULL);
}

void __initialize_engine()
{
    SDL_LogSetPriority(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_DEBUG); // Enable SDL error logging
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0"); // Don't interpret touch events as mouse events
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0"); // Something that... Aparently.. does.. nothing....

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        ERROR("SDL ERROR: Could not initialize SDL Subsystems: '%s'", SDL_GetError());
        exit(-1);
    }
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    __init_containers();

    const struct _win_settings _win = __setup_window_data();
    __setup_audio_device();
    __setup_engine_data();

    __create_window(&_win);
    __setup_renderer(_win.renderflags);
    __init_actions();

    atexit(__end_engine);
}

void __end_engine()
{
    __clean_containers();    
    
    SDL_CloseAudioDevice(engine.audio_device);
    SDL_DetachThread(engine.entry_thread);
    SDL_DestroyTexture(engine.sdl_rectTex);
    SDL_DestroyMutex(engine.sdl_rendermutex);
    SDL_DestroyMutex(engine.destroyqueue_mutex);
    SDL_DestroyWindow(engine.sdl_windowhndl);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}