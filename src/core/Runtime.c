#include <OS.h>
#include <stdio.h>
#include "../internal.h"

#include <utils/logging.h>

#include <Core/Defer.h>

static int __event_watch(void *, SDL_Event *);

static int game_loop(void* running)
{
    while (SDL_AtomicGet(running))
    {
        engine.frame++;

        engine.frameend_time = os.clock();
        engine.last_dt = (engine.frameend_time - engine.framestart_time) / (sre_timeStamp)CLOCK_FREQUENCY;
        engine.framestart_time = engine.frameend_time;

        //
        SDL_LockMutex(engine.render_mutex);
            __queue_events();
            
            __update_threads();
            __destroy_queue();

            __query_objects();
            __update_world();
            __update_layer();

            __destroy_queue();

        SDL_Event ev;
        ev.type = SDL_USEREVENT;
        ev.user.code = ENGINE_EVENT_RENDER;
        SDL_PushEvent(&ev);
        SDL_CondWait(engine.render_cond, engine.render_mutex);
        SDL_UnlockMutex(engine.render_mutex);
        //

        sre_timeStamp elapsed;
        elapsed = (os.clock() - engine.framestart_time) / (sre_timeStamp)CLOCK_FREQUENCY;
        elapsed = engine.target_dt - elapsed;

        if (elapsed > 0)
            delay_s(elapsed);
    }

    return 0;
}

void __run_engine()
{
    SDL_atomic_t running = {1};
    engine.game_loop = SDL_CreateThread(game_loop, "Game Loop", &running);

    SDL_AddEventWatch(__signal_events, NULL);
    SDL_AddEventWatch(__event_watch, NULL);

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
            case ENGINE_EVENT_RETDEFER:
                struct _engine_retdefer* defer = ev.user.data2;
                defer->ret = ((sre_deferResponseFunction)ev.user.data1)(defer->userdata);
                SDL_SemPost(defer->sem);
                break;
            case ENGINE_EVENT_RENDER:
                SDL_LockMutex(engine.render_mutex);
                    __display_render();
                SDL_UnlockMutex(engine.render_mutex);
                SDL_CondBroadcast(engine.render_cond);
                break;
            case ENGINE_EVENT_ENTRY:
                SDL_ShowWindow(engine.sdl_windowhndl);
                SDL_RaiseWindow(engine.sdl_windowhndl);
                __update_viewport(engine.osize_x, engine.osize_y);
                break;
            }
            break;
        default:
            __poll_input(&ev);
            break;
        }
    }

    
    SDL_AtomicSet(&running, 0);
    SDL_DestroyCond(engine.render_cond); // Destroy cond here before waiting for thread
    SDL_WaitThread(engine.game_loop, NULL);
}

static int __event_watch(void *data, SDL_Event *ev)
{
    if (ev->type == SDL_WINDOWEVENT)
    {
        switch (ev->window.event)
        {
            #if _WIN32
            case SDL_WINDOWEVENT_EXPOSED:
                if (SDL_TryLockMutex(engine.render_mutex) == 0)
                {
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