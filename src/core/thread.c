#include <Base/Thread.h>
#include <utils/mem.h>

#include "../internal.h"

#include <OS.h>

typedef struct sre_threadinst
{
    sre_sptr id;

    SDL_Thread* handle;
    sre_sptr (*function)(void* data);
    void* data;

    struct sre_threadinst* next;
} sre_threadinst;

static sre_sptr thread_entry(sre_threadinst* inst)
{
    sre_sptr id = SDL_ThreadID();

    sre_threadinst** current = &((sre_threadinst**)engine.threads_bucket)[id % SRE_THREADS_BUCKETSIZE];
    if (!*current) *current = inst;
    else
    {
        while ((*current)->next)
            current = &(*current)->next;
        *current = inst;
    }

    sre_sptr res = inst->function(inst->data);

    // Perform some cleanup...

    return res;
}

sre_Thread sre_threadcreate_delaystacksize(sre_sptr (*function)(void* data), void* data, sre_timeStamp delay, sre_usize stacksize)
{
    if (!engine.threads_bucket)
    {
        engine.threads_bucket = sre_new(sizeof(sre_threadinst*) * SRE_THREADS_BUCKETSIZE);
        memset(engine.threads_bucket, 0, sizeof(sre_threadinst*) * SRE_THREADS_BUCKETSIZE);
    }

    sre_threadinst* inst = sre_new(sizeof(sre_threadinst*));
    inst->next = NULL;
    inst->function = function;
    inst->data = data;
    inst->handle = SDL_CreateThreadWithStackSize((SDL_ThreadFunction)thread_entry, "Story Royale Engine Thread", stacksize, inst);

    return SRE_INVALIDTHREAD;
}