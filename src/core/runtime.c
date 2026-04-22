#include <stdio.h>
#include "../internal.h"

#include <Core/Defer.h>
#include <Base/Log.h>

#if _WIN32 && 1
    #define WIN32_HANDLE_WINDOW_BLOCKING
#endif

#if _WIN32
    #include "time/win32.c"

    #ifdef WIN32_HANDLE_WINDOW_BLOCKING
        static CONDITION_VARIABLE win32_rendercond;
        static SRWLOCK win32_rendersrw;

        static void win32_renderflush(void* userdata)
        {
            AcquireSRWLockExclusive(&win32_rendersrw);
            if (!engine.quit) __render_flush();
            WakeAllConditionVariable(&win32_rendercond);
            ReleaseSRWLockExclusive(&win32_rendersrw);
        }
    #endif
#elif __unix__
    #include "time/unix.c"
#else
    #error "Implement your own sleep!!"
#endif

#ifndef WIN32_HANDLE_WINDOW_BLOCKING
        static SDL_sem* sdl_rendersem;
        static void sem_renderflush(void* userdata)
        {
            assert(sdl_rendersem != NULL);
            __render_flush();
            SDL_SemPost(sdl_rendersem);
        }
#endif

static int __event_watch(void *, SDL_Event *);

static int game_loop(void* running)
{
    const long long FREQUENCY = frequency();
    ticks(&engine.framestart_time);

    #ifdef WIN32_HANDLE_WINDOW_BLOCKING
        AcquireSRWLockExclusive(&win32_rendersrw);
    #endif

    while (*(int*)running)
    {
        sre_timeStamp elapsed;
        SDL_Event ev;
        ev.type = SDL_USEREVENT;
        ev.user.code = ENGINE_EVENT_RENDER;
        
        engine.frame++;

        ticks(&engine.frameend_time);
        engine.last_dt = (engine.frameend_time - engine.framestart_time) / SRE_TS(FREQUENCY);
        if (!engine.last_dt) continue;
        assert(engine.last_dt > 0);

        engine.framestart_time = engine.frameend_time;

        //

        __queue_events();
        
        #ifdef WIN32_HANDLE_WINDOW_BLOCKING
            if (__update_ecs())
            {
                sre_defer(win32_renderflush, NULL);
                SleepConditionVariableSRW(&win32_rendercond, &win32_rendersrw, 5000, 0);
            }
        #else
            if (__update_ecs())
            {
                sre_defer(sem_renderflush, NULL);
                SDL_SemWait(sdl_rendersem);
            }
        #endif

       
        ticks(&engine.frameend_time);
        elapsed = (engine.frameend_time - engine.framestart_time) / SRE_TS(FREQUENCY);
        elapsed = engine.target_dt - elapsed;

        if (elapsed > 0)
            wait(elapsed);
    }

    return 0;
}

#ifdef WIN32_HANDLE_WINDOW_BLOCKING

    static int win32_eventwatch(void* userdata, SDL_Event* ev)
    {
        if (ev->type == SDL_QUIT)
            engine.quit = 1;

        SDL_threadID thrd = SDL_ThreadID();
        if (thrd != engine.main_thrd)
            return 1;

        if (ev->type == SDL_WINDOWEVENT)
        {
            switch (ev->window.event)
            {
                case SDL_WINDOWEVENT_EXPOSED:
                    if (TryAcquireSRWLockExclusive(&win32_rendersrw))
                    {
                        __render_flush();
                        ReleaseSRWLockExclusive(&win32_rendersrw);
                    }
                    WakeAllConditionVariable(&win32_rendercond);
                    break;
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    __update_viewport(ev->window.data1, ev->window.data2);
                    break;
            }
        }

        return 1;
    }

#endif

void __run_engine()
{
    SDL_RegisterEvents(1);

    static int running = 1;
    SDL_Thread* gameloop = SDL_CreateThread(game_loop, "Game Loop", &running);

    #ifdef WIN32_HANDLE_WINDOW_BLOCKING
        InitializeConditionVariable(&win32_rendercond);
        InitializeSRWLock(&win32_rendersrw);
        SDL_SetEventFilter(win32_eventwatch, NULL);
    #else
        sdl_rendersem = SDL_CreateSemaphore(0);
    #endif

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
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                __update_viewport(ev.window.data1, ev.window.data2);
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
            case ENGINE_EVENT_ENTRY: {
                    int w, h;
                    SDL_GetWindowSize(engine.sdl_windowhndl, &w, &h);
                    __update_viewport(w, h);

                    SDL_ShowWindow(engine.sdl_windowhndl);
                    SDL_RaiseWindow(engine.sdl_windowhndl);
                } break;
            }
            break;
        default:
            __poll_input(&ev);
            break;
        }

        __signal_events(&ev); // In this case the QUIT event won't be fired
                              // I'll make sure to make it fire once I make the QUIT event
    }

    running = 0;
    #ifdef WIN32_HANDLE_WINDOW_BLOCKING
        SDL_WaitThread(gameloop, NULL);
    #else
        SDL_SemPost(sdl_rendersem);
        SDL_WaitThread(gameloop, NULL);
        SDL_DestroySemaphore(sdl_rendersem);
    #endif
}