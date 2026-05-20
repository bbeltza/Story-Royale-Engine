#include <Core/Defer.h>
#include <Base/Error.h>
#include <utils/mem.h>

#include "../internal.h"

bool sre_defer(sre_deferFunction func, size_t argsize, const void* arg)
{
    if (!engine.user_event)
        return false;

    if (!engine.main_thrd || SDL_ThreadID() == engine.main_thrd)
    {
        if (argsize && arg)
        {
            ut_dynsalloc(sre_byte, argarr, argsize);
            memcpy(argarr, arg, argsize);
            func(argarr);
        }
        else
            func((void*)arg);

        return true;
    };

    SDL_Event ev = { .type = engine.user_event };
    
    ev.user.data1 = func;
    if (argsize == 0 || !arg)
    {
        ev.user.code = ENGINE_EVENT_DEFERV;
        ev.user.data2 = (void*)arg;    
    }
    else if (argsize <= sizeof(void*))
    {
        ev.user.code = ENGINE_EVENT_DEFERP;
        memcpy(&ev.user.data2, arg, argsize);
    }
    else
    {
        ev.user.code = ENGINE_EVENT_DEFERPM;
        ev.user.data2 = sre_new(argsize);
        memcpy(ev.user.data2, arg, argsize);
    }

    int res = SDL_PushEvent(&ev);
    if (res != 1)
        return sre_error(SRE_ERR_SDL, SDL_GetError()) && false;

    return true;
}

static void dr_tlsdestructor(void* _data)
{
    void** data = _data;
    SDL_DestroySemaphore(data[0]);
    sre_delete(_data);
}

intptr_t sre_defer_res(sre_deferresFunction func, void* arg)
{
    if (!engine.user_event)
        return -1;
    if (!engine.main_thrd || SDL_ThreadID() == engine.main_thrd)
        return func(arg);

    static SDL_TLSID s_tlsid;
    if (!s_tlsid)
    {
        s_tlsid = SDL_TLSCreate();
    }

    // user.data2 will recieve a thread-specific pointer to an array of 2 pointers:
        // - The first pointer contains the semaphore that the main thread will have to post to resume the calling thread.
        // - The second pointer contains the user pointer passed in sre_defer_res. It is not copied, since it assumes that the data inside it is managed by the calling thread, so it assumes that no state in `arg` will be changed
        // - The second pointer will be written with `func`'s return value after it returns, so this function will return (intptr_t)deferdata[1]
    void** deferdata = SDL_TLSGet(s_tlsid);
    if (!deferdata)
    {
        deferdata = sre_new(sizeof(void*)*2);
        deferdata[0] = SDL_CreateSemaphore(0);
        SDL_TLSSet(s_tlsid, deferdata, dr_tlsdestructor);
    }

    SDL_Event ev = { .type = engine.user_event };
    ev.user.code = ENGINE_EVENT_DEFERRES;
    ev.user.data1 = func;
    ev.user.data2 = deferdata;
    deferdata[1] = (void*)arg;

    int res = SDL_PushEvent(&ev);
    if (res != 1)
        return sre_error(SRE_ERR_SDL, SDL_GetError());

    if (SDL_SemWait(deferdata[0]) != 0)
        return -1;
    
    return (intptr_t)deferdata[1];
}