#include <SDL_ttf.h>
#include <SDL_image.h>
#include "../internal.h"

#include "GameEntry.h"

#include <stdlib.h>
#include <stdio.h>

#include <utils/logging.h>

static int __invoke_entry(void* userdata)
{
    SDL_Event finish_event = { 0 };
    finish_event.type = SDL_USEREVENT;
    finish_event.user.code = 1;

    Initialize();
    SDL_PushEvent(&finish_event);

    return 0;
}

inline void __setup_engine_data()
{
    engine.phys_target_dt = 1.0 / 64.0;

    engine.input_last_touchid = -1;
    engine.destroyqueue_mutex = SDL_CreateMutex();
    engine.entry_thread = SDL_CreateThread(__invoke_entry, "Game Entry", NULL);
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

    __setup_audio_device();
    __create_window();
    __setup_renderer();

    __setup_engine_data();

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