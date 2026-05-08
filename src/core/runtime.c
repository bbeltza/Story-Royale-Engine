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
        static SDL_cond* win32_rendercond;
        static SDL_mutex* win32_rendermtx;
        static bool win32_expose;

        static void win32_renderflush(void* userdata)
        {
            int res;
            (void)res;
            while (SDL_TryLockMutex(win32_rendermtx) != 0)
            {
                if (win32_expose)
                    goto ENDWAKE;
            }
            __render_flush();
            SDL_UnlockMutex(win32_rendermtx);

        ENDWAKE:
            SDL_CondBroadcast(win32_rendercond);
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
        int res = SDL_LockMutex(win32_rendermtx);
        assert(res == 0);
    #endif

    while (*(volatile int*)running)
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
        
        engine.video.wantclear = true;
        #ifdef WIN32_HANDLE_WINDOW_BLOCKING
            if (__update_ecs())
            {
                sre_defer(win32_renderflush, 0, NULL);
                int res = SDL_CondWaitTimeout(win32_rendercond, win32_rendermtx, 5000);
                assert(res >= 0);
            }
        #else
            if (__update_ecs())
            {
                sre_defer(sem_renderflush, 0, NULL);
                SDL_SemWait(sdl_rendersem);
            }
        #endif

       
        ticks(&engine.frameend_time);
        elapsed = (engine.frameend_time - engine.framestart_time) / SRE_TS(FREQUENCY);
        elapsed = engine.target_dt - elapsed;

        if (elapsed > 0)
            wait(elapsed);
    }

    #ifdef WIN32_HANDLE_WINDOW_BLOCKING
        SDL_UnlockMutex(win32_rendermtx);
    #endif
    return 0;
}

#if _WIN32
static int win32_eventwatch(void* userdata, SDL_Event* ev)
{   
    #ifdef WIN32_HANDLE_WINDOW_BLOCKING
            SDL_threadID thrd = SDL_ThreadID();
            if (thrd != engine.main_thrd)
                return 1;

            if (ev->type == SDL_WINDOWEVENT)
            {
                switch (ev->window.event)
                {
                    case SDL_WINDOWEVENT_EXPOSED:
                        if (SDL_TryLockMutex(win32_rendermtx) == 0)
                        {
                            win32_expose = true;
                            __render_flush();
                            SDL_UnlockMutex(win32_rendermtx);
                        }
                        SDL_CondBroadcast(win32_rendercond);
                        break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        __update_viewport(ev->window.data1, ev->window.data2);
                        break;
                }
            }
    #endif
    return 1;
}
#endif

void __run_engine()
{

    #if _WIN32
        SDL_AddEventWatch(win32_eventwatch, NULL);
    #endif
    #ifdef WIN32_HANDLE_WINDOW_BLOCKING
        win32_rendercond = SDL_CreateCond();
        win32_rendermtx = SDL_CreateMutex();
    #else
        sdl_rendersem = SDL_CreateSemaphore(0);
    #endif
    static int running = 1;
    SDL_Thread* gameloop = SDL_CreateThread(game_loop, "Game Loop", &running);

    SDL_Event ev;
    while (SDL_WaitEvent(&ev))
    {
        if (ev.type == SDL_QUIT)
        {
            running = 0;
            break;
        }

        if (ev.type == engine.user_event)
        {
            switch (ev.user.code)
            {
            case ENGINE_EVENT_DEFERV:
                ((sre_deferFunction)ev.user.data1)(ev.user.data2);
                break;
            case ENGINE_EVENT_DEFERP:
                ((sre_deferFunction)ev.user.data1)(&ev.user.data2);
                break;
            case ENGINE_EVENT_DEFERPM:
                extern void sre_delete(void*);
                ((sre_deferFunction)ev.user.data1)(ev.user.data2);
                sre_delete(ev.user.data2);
                break;
            case ENGINE_EVENT_DEFERRES: {
                void** deferdata = ev.user.data2;
                deferdata[1] = (void*)((sre_deferresFunction)ev.user.data1)(deferdata[1]);
                SDL_SemPost(deferdata[0]);
            } break;
            case ENGINE_EVENT_ENTRY: {
                    int w, h;
                    SDL_GetWindowSize(engine.sdl_windowhndl, &w, &h);
                    __update_viewport(w, h);

                    SDL_ShowWindow(engine.sdl_windowhndl);
                    SDL_RaiseWindow(engine.sdl_windowhndl);
                } break;
            }
            continue;
        }

        switch (ev.type)
        {
        case SDL_WINDOWEVENT:
            switch (ev.window.event)
            {
            #ifdef WIN32_HANDLE_WINDOW_BLOCKING
                case SDL_WINDOWEVENT_EXPOSED:
                    win32_expose = false;
                    break;
            #endif
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                __update_viewport(ev.window.data1, ev.window.data2);
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                SDL_ResetKeyboard();
                break;
            }
            break;
        default:
            __poll_input(&ev);
            break;
        }

        if (engine.imgui && !__onevent_imgui(&ev))
            continue;
        
        __signal_events(&ev); // In this case the QUIT event won't be fired
                              // I'll make sure to make it fire once I make the QUIT event
    }

    #ifdef WIN32_HANDLE_WINDOW_BLOCKING
        SDL_LockMutex(win32_rendermtx);
        SDL_CondBroadcast(win32_rendercond);
        SDL_UnlockMutex(win32_rendermtx);
        SDL_WaitThread(gameloop, NULL);

        SDL_DestroyCond(win32_rendercond);
        SDL_DestroyMutex(win32_rendermtx);
    #else
        SDL_SemPost(sdl_rendersem);
        SDL_WaitThread(gameloop, NULL);
        SDL_DestroySemaphore(sdl_rendersem);
    #endif
}