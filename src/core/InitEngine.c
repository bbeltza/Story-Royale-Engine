#include <SDL_ttf.h>
#include <SDL_image.h>
#include "../internal.h"

#include "drivers/drivers.h"

#include <Entry.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <utils/logging.h>
#include <utils/mem.h>
#include <OS.h>

extern bool sre_coroutinecoreinit();
extern void sre_coroutinecorequit();

static int __invoke_entry(void* userdata) // Invoking the entry-point won't be a thread anymore. It'll actually be a coroutine
{
    SDL_Event finish_event = { 0 };
    finish_event.type = SDL_USEREVENT;
    finish_event.user.code = 1;
    
    sre_initialize();
    SDL_PushEvent(&finish_event);

    return 0;
}

static inline void __setup_engine_data()
{
    engine.phys_target_dt = 1 / 128.0;
    engine.framestart_time = os.clock();

    engine.input_last_touchid = -1;
    engine.destroyqueue_mutex = SDL_CreateMutex();
    engine.main_thrd = SDL_ThreadID();
    engine.entry_thread = SDL_CreateThread(__invoke_entry, "Game Entry", NULL);
}

#define SDLPCASE(x, f) case SDL_LOG_PRIORITY_##x: priority_str = #x"]: "; files[0] = f; break
#define SDLCCASE(x) case SDL_LOG_CATEGORY_##x: category_str = "["#x" : "; break
static void sdl_log_callback(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    const char *priority_str, *category_str;
    FILE* files[] = {
        NULL,
        *SRE_LOGFILE,
        NULL
    };

    switch (priority)
    {
        SDLPCASE(DEBUG, stdout);
        SDLPCASE(INFO, stdout);
        SDLPCASE(WARN, stderr);
        SDLPCASE(ERROR, stderr);
        SDLPCASE(CRITICAL, stderr);
        default:
            return;
    }

    switch (category)
    {
        SDLCCASE(APPLICATION);
        SDLCCASE(ERROR);
        SDLCCASE(ASSERT);
        SDLCCASE(SYSTEM);
        SDLCCASE(AUDIO);
        SDLCCASE(VIDEO);
        SDLCCASE(RENDER);
        SDLCCASE(INPUT);
        SDLCCASE(TEST);
        SDLCCASE(CUSTOM);
    default:
        return;
    }

    char full_prefix[255] = "[SDL]";

    strncat(full_prefix, category_str, 255 - strlen(full_prefix) - strlen(category_str));
    strncat(full_prefix, priority_str, 255 - strlen(full_prefix) - strlen(priority_str));

    CUSTOM_LOG(full_prefix, files, message, NULL, '\1');
}

void __initialize_engine()
{
    SDL_LogSetPriority(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_DEBUG); // Enable SDL error logging
    SDL_LogSetOutputFunction(sdl_log_callback, NULL);
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0"); // Don't interpret touch events as mouse events
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0"); // Something that... Aparently.. does.. nothing....

#ifdef SDL_VIDEO_DRIVER_WINDOWS // Replace the "SDL_app" window class name
    SDL_RegisterApp("Story Royale Engine", 0, NULL);
#endif

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        ERROR("SDL ERROR: Could not initialize SDL Subsystems: '%s'", SDL_GetError());
        exit(-1);
    }
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    sre_coroutinecoreinit();

    __setup_audio_device();
    __create_window();
    __setup_renderer();

    __setup_engine_data();

    atexit(__end_engine);
}

void __end_engine()
{
    sre_coroutinecorequit();
    __cleanup_threads();
    __clean_containers();  
    
    engine.video->quit(engine.video);
    sre_delete((void*)engine.video->texture_fl);
    sre_delete(engine.video->textures);
    sre_delete(engine.video);
    
    engine.video = NULL;
    
    SDL_CloseAudioDevice(engine.audio_device);
    SDL_DetachThread(engine.entry_thread);
    SDL_DestroyMutex(engine.sdl_rendermutex);
    SDL_DestroyMutex(engine.destroyqueue_mutex);
    SDL_DestroyWindow(engine.sdl_windowhndl);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    LOG("SDL_GetNumAllocations(): %d", SDL_GetNumAllocations());

    if (*SRE_LOGFILE)
        fclose(*SRE_LOGFILE);
}