#include <SDL_ttf.h>
#include <SDL_image.h>
#include "../internal.h"

#include "drivers/drivers.h"

#include <Entry.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <utils/mem.h>

#include <Base/Log.h>
#include <Base/Coroutine.h>

extern bool sre_coroutinecoreinit();
extern void sre_coroutinecorequit();

static void __invoke_entry(void* userdata) // Invoking the entry-point won't be a thread anymore. It'll actually be a coroutine
{
    SDL_Event finish_event = { 0 };
    finish_event.type = SDL_USEREVENT;
    finish_event.user.code = ENGINE_EVENT_ENTRY;
    
    sre_initialize();
    SDL_PushEvent(&finish_event);
}

static int __run_coroutine(void* run)
{
    _coroutine_coreinit(run);

    return 0;
}

static inline void __setup_engine_data()
{
    engine.phys_target_dt = 1 / 128.0;

    engine.input_last_touchid = -1;
    engine.destroyqueue_mutex = SDL_CreateMutex();

    engine.main_thrd = SDL_ThreadID();

    engine.cor_running = true;
    engine.coroutine_thread = SDL_CreateThread(__run_coroutine, "Coroutine Engine", &engine.cor_running);
    sre_coroutinecreate(false, __invoke_entry, NULL);
}

static void sdl_log_callback(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    int cat;
    switch (category)
    {
    case SDL_LOG_CATEGORY_ERROR:
        cat = SRE_LOGCATEGORY_ERROR;
        break;
    default:
        cat = SRE_LOGCATEGORY_DEBUG;
        break;
    }

    sre_logsimpleEx(2, cat, message);
}

void __initialize_engine()
{
    SDL_LogSetPriority(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_DEBUG); // Enable SDL error logging
    SDL_LogSetOutputFunction(sdl_log_callback, NULL);
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0"); // Don't interpret touch events as mouse events
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0"); // Something that... Aparently.. does.. nothing....

#ifdef SDL_VIDEO_DRIVER_WINDOWS // Replace the "SDL_app" window class name
    SDL_RegisterApp("Story Royale Engine", 0x0003, NULL);
#endif

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        sre_log(SRE_LOGCATEGORY_ERROR, "SDL ERROR: Could not initialize SDL Subsystems: '%s'", SDL_GetError());
        exit(-1);
    }
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    __setup_audio_device();
    __create_window();
    __setup_renderer();

    __setup_engine_data();

    atexit(__end_engine);
}

void __end_engine()
{
    engine.cor_running = false;
    SDL_WaitThread(engine.coroutine_thread, NULL);

    __cleanup_ecs();  
    
    __cleanup_renderer();
    
    SDL_CloseAudioDevice(engine.audio_device);
    SDL_DestroyMutex(engine.destroyqueue_mutex);
    SDL_DestroyWindow(engine.sdl_windowhndl);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    //sre_log("SDL_GetNumAllocations(): %d", SDL_GetNumAllocations());
}