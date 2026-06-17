#include <SDL_image.h>
#include "../internal.h"

#include <Entry.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <utils/mem.h>

#include <Base/Log.h>
#include <Base/Error.h>
#include <Base/Coroutine.h>

#include <Hints.h>

static void __invoke_entry(void* userdata) // Invoking the entry-point won't be a thread anymore. It'll actually be a coroutine
{
    SDL_Event finish_event = { 0 };
    finish_event.type = engine.user_event;
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
    engine.user_event = SDL_RegisterEvents(1);
    if (engine.user_event == (Uint32)-1)
        sre_CRITICAL(SRE_ERR_CORE, "SDL_RegisterEvents(1) failed and returned -1 ; This means there are no available user events to register... Which is required for the engine to work");

    engine.input_last_touchid = -1;

    engine.main_thrd = SDL_ThreadID();

    engine.cor_running = true;
    engine.coroutine_thread = SDL_CreateThread(__run_coroutine, "Coroutine Engine", &engine.cor_running);
    sre_coroutinecreate(false, __invoke_entry, NULL);
}

static void sdl_log_callback(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    extern int sre_internallogmsg(int type, int category, const char* msg, int length);

    int cat;
    switch (category)
    {
    case SDL_LOG_CATEGORY_ERROR:
        cat = '\x03';
        break;
    default:
        cat = '\x00';
        break;
    }

    sre_internallogmsg(2, cat, message, 0);
}

void __initialize_engine()
{
    SDL_LogSetPriority(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_DEBUG); // Enable SDL error logging
    SDL_LogSetOutputFunction(sdl_log_callback, NULL);
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0"); // Don't interpret touch events as mouse events
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0"); // Something that... Aparently.. does.. nothing....

#ifdef SDL_VIDEO_DRIVER_WAYLAND // Use wayland, if possible
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland");
#endif
#ifdef SDL_VIDEO_DRIVER_WINDOWS // Replace the "SDL_app" window class name
    SDL_RegisterApp("Story Royale Engine", 0x0003, NULL);
#endif

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        sre_error(SRE_ERR_CORE, "Could not initialize SDL Subsystems");
        sre_error(SRE_ERR_SDL, SDL_GetError());
        exit(-1);
    }
    
    IMG_Init(IMG_INIT_PNG); 

    __setup_audio_device();
    __create_window();
    __setup_renderer();

    __setup_objects();

    __setup_engine_data();

    atexit(__end_engine);
}

void __end_engine()
{
    if (SDL_ThreadID() != engine.main_thrd)
        sre_CRITICAL(SRE_ERR_FAIL, "Engine destruction NOT being run on the main thread. This will thus cause lots of issues with object destruction. Are you trying to call exit()? Try with wrapping it into a deferred function with sre_defer/sre::defer");

    engine.cor_running = false;
    SDL_WaitThread(engine.coroutine_thread, NULL);
    
    __cleanup_objects();

    { // Send a quit event
        SDL_Event quit_ev;
        quit_ev.quit.type = SDL_QUIT;
        quit_ev.quit.timestamp = SDL_GetTicks();
        __signal_events(&quit_ev);
        __queue_events();
    }

    __cleanup_renderer();
    
    SDL_CloseAudioDevice(engine.audio_device);
    SDL_DestroyWindow(engine.sdl_windowhndl);

    IMG_Quit();
    SDL_Quit();

    //sre_log("SDL_GetNumAllocations(): %d", SDL_GetNumAllocations());
}