#include <Base/Defer.h>

#include "../internal.h"

struct sre_defer
{
    sre_deferFunction func;
    void* userdata;
    struct sre_defer* next;
};

struct sre_retdefer
{
    struct sre_defer defer;
    SDL_sem* sem;
    sre_sptr ret;
};

void sre_defer(sre_deferFunction func, void* userdata)
{
    struct sre_defer* defer = malloc(sizeof(struct sre_defer));
    defer->func = func;
    defer->userdata = userdata;
    defer->next = engine.defer_head;

    engine.defer_head = defer;
}

sre_sptr sre_defer_response(sre_deferResponseFunction func, void* userdata)
{
    if (SDL_ThreadID() == engine.main_thrd) return func(userdata);

    sre_sptr ret;
    struct sre_retdefer* defer = malloc(sizeof(struct sre_retdefer));
    defer->defer.func = (sre_deferFunction)func;
    defer->defer.userdata = userdata;
    defer->defer.next = engine.retdefer_head;
    defer->sem = SDL_CreateSemaphore(0);
    
    engine.retdefer_head = defer;

    SDL_SemWait(defer->sem);

    ret = defer->ret;

    SDL_DestroySemaphore(defer->sem);
    free(defer);

    return ret;
}

void __call_deferred()
{
    while (engine.defer_head)
    {
        struct sre_defer* defer = engine.defer_head;
        defer->func(defer->userdata);

        engine.defer_head = defer->next;

        free(defer);
    }

    while (engine.retdefer_head)
    {
        struct sre_retdefer* defer = engine.retdefer_head;
        defer->ret = ((sre_deferResponseFunction)defer->defer.func)(defer->defer.userdata);
        engine.retdefer_head = defer->defer.next;

        SDL_SemPost(defer->sem);
    }
}