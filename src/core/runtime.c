#include <stdio.h>
#include "../internal.h"

#include <Core/Defer.h>
#include <Base/Log.h>

#if _WIN32
    #include "time/win32.c"
#elif __unix__
    #include "time/unix.c"
#else
    #error "Implement your own sleep!!"
#endif

extern void sre_logflush();
static int __event_watch(void *, SDL_Event *);

static int game_loop(void* running)
{
    const long long FREQUENCY = frequency();
    ticks(&engine.framestart_time);

    SDL_LockMutex(engine.render_mutex); // Lock the mutex once, it will be unlocked every time SDL_CondWait gets called
    while (*(int*)running)
    {
        sre_timeStamp elapsed;
        SDL_Event ev;
        ev.type = SDL_USEREVENT;
        ev.user.code = ENGINE_EVENT_RENDER;

        engine.frame++;

        ticks(&engine.frameend_time);
        engine.last_dt = (engine.frameend_time - engine.framestart_time) / SRE_TS(FREQUENCY);
        engine.framestart_time = engine.frameend_time;

        //

        __queue_events();
        __update_threads();
        __update_ecs();

        //

    #if _WIN32
        if (!engine.exposing)
        {
    #endif
        
        if (SDL_PeepEvents(&ev, 1, SDL_ADDEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) == 1)
            SDL_CondWaitTimeout(engine.render_cond, engine.render_mutex, 5000);
        else
            sre_log(SRE_LOGCATEGORY_ERROR, "SDL_PeepEvents failed... %s", SDL_GetError());
    #if _WIN32
        }
        else
            SDL_CondWait(engine.render_cond, engine.render_mutex);
    #endif

        //
        
        ticks(&engine.frameend_time);
        elapsed = (engine.frameend_time - engine.framestart_time) / SRE_TS(FREQUENCY);
        elapsed = engine.target_dt - elapsed;

        if (elapsed > 0)
            wait(elapsed);
    }

    return 0;
}

void __run_engine()
{
    int running = 1;
    engine.game_loop = SDL_CreateThread(game_loop, "Game Loop", &running);

    SDL_SetEventFilter(__event_watch, NULL);

    SDL_Event ev;
    while (SDL_WaitEvent(&ev))
    {
        if (ev.type == SDL_QUIT)
            break;
        
        switch (ev.type)
        {
        case SDL_WINDOWEVENT:
            switch (ev.window.event)
            {
            case SDL_WINDOWEVENT_CLOSE:
                sre_log(SRE_LOGCATEGORY_DEBUG, "close");
                break;
            case SDL_WINDOWEVENT_EXPOSED:
                #if _WIN32
                    engine.exposing = false;
                #endif
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                SDL_ResetKeyboard();
                break;
            }
            break;
        case SDL_USEREVENT:
            switch (ev.user.code)
            {
            case ENGINE_EVENT_DEFER:
                ((sre_deferFunction)ev.user.data1)(ev.user.data2);
                break;
            case ENGINE_EVENT_RETDEFER: {
                struct _engine_retdefer* defer = ev.user.data2;
                defer->ret = ((sre_deferResponseFunction)ev.user.data1)(defer->userdata);
                SDL_SemPost(defer->sem);
            } break;
            case ENGINE_EVENT_RENDER:
                SDL_LockMutex(engine.render_mutex);

                __display_render();

                if (SDL_CondBroadcast(engine.render_cond) < 0) assert(0 && "This should not happen!");
                SDL_UnlockMutex(engine.render_mutex);
                break;
            case ENGINE_EVENT_ENTRY:
                __update_viewport(engine.osize_x, engine.osize_y);
                SDL_ShowWindow(engine.sdl_windowhndl);
                SDL_RaiseWindow(engine.sdl_windowhndl);
                break;
            }
            break;
        default:
            __poll_input(&ev);
            break;
        }

        __signal_events(&ev); // In this case the QUIT event won't be fired
                              // I'll make sure to make it fire once I make the QUIT event
    }

    
    SDL_LockMutex(engine.render_mutex);
        running = 0;
        SDL_CondBroadcast(engine.render_cond);
        SDL_DestroyCond(engine.render_cond); // Destroy cond and mutex here before waiting for thread
    SDL_UnlockMutex(engine.render_mutex);

    SDL_WaitThread(engine.game_loop, NULL);

    SDL_DestroyMutex(engine.render_mutex);
}

static int __event_watch(void *data, SDL_Event *ev)
{
    SDL_threadID id = SDL_ThreadID();
    if (id != engine.main_thrd)
        return 1;

    //if (ev->type == SDL_QUIT) __debugbreak();
    if (ev->type == SDL_WINDOWEVENT)
    {
        switch (ev->window.event)
        {
            #if _WIN32
            case SDL_WINDOWEVENT_EXPOSED:
                if (SDL_TryLockMutex(engine.render_mutex) == 0)
                {
                    engine.exposing = true;
                    __display_render();
                    SDL_UnlockMutex(engine.render_mutex);
                }
                SDL_CondBroadcast(engine.render_cond);
                break;
            #endif
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                __update_viewport(ev->window.data1, ev->window.data2);
                break;
        }
    }

    return 1;
}