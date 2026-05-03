#include <Core/Defer.h>
#include <utils/mem.h>

#include "../internal.h"

void sre_defer(sre_deferFunction func, void* userdata)
{
    if (SDL_ThreadID() == engine.main_thrd){
        func(userdata);
        return;
    };

    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    ev.user.code = ENGINE_EVENT_DEFER;
    ev.user.data1 = func;
    ev.user.data2 = userdata;
    int res = SDL_PushEvent(&ev);
    assert(res == 1);
    (void)res;
}

sre_sptr sre_defer_response(sre_deferResponseFunction func, void* userdata)
{
    if (!engine.main_thrd || SDL_ThreadID() == engine.main_thrd) return func(userdata);

    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    ev.user.code = ENGINE_EVENT_RETDEFER;
    ev.user.data1 = func;
    
    sre_sptr ret;
    struct _engine_retdefer* defer = sre_new(sizeof(struct _engine_retdefer));
    defer->userdata = userdata;
    defer->sem = SDL_CreateSemaphore(0);
    
    ev.user.data2 = defer;
    int res = SDL_PushEvent(&ev);
    assert(res == 1);
    (void)res;

    SDL_SemWait(defer->sem);
    ret = defer->ret;

    SDL_DestroySemaphore(defer->sem);
    sre_delete(defer);

    return ret;
}