#include <Base/Thread.h>
#include <utils/mem.h>

#include "../internal.h"
#define thread_bucket ((sre_threadinst**)engine.threads_bucket)

#include <OS.h>
#include <utils/logging.h>

typedef struct sre_threadinst
{
    sre_Thread id;
    SDL_Thread* handle;

    sre_sptr (*function)(void* data);
    void* data;

    struct sre_threadinst* next;

    sre_timeStamp delay;
} sre_threadinst;

void __update_threads()
{
    if (!engine.threads_bucket) return;

    for (size_t i = 0; i < SRE_THREADS_BUCKETSIZE; i++)
    {
        sre_threadinst* prev = NULL;
        sre_threadinst* curr = thread_bucket[i];
        if (curr == NULL) continue;

        do
        {
            if (curr->function) // curr->function == NULL means the thread has finished executing its function and is now to be joined
            {
                prev = curr;
                curr = curr->next;
                continue;
            }

            if (prev)
                prev->next = curr->next;
            else
                thread_bucket[i] = curr->next;

            sre_threadinst* tmpcurr = curr;
            curr = curr->next;

            SDL_WaitThread(tmpcurr->handle, NULL);
            sre_delete(tmpcurr);
        } while (curr);
    }
}

void __cleanup_threads()
{
    for (size_t i = 0; i < SRE_THREADS_BUCKETSIZE; i++)
    {
        sre_threadinst* curr = thread_bucket[i];
        while (curr)
        {
            SDL_DetachThread(curr->handle);

            void* ptr = curr;
            curr = curr->next;

            sre_delete(ptr);
        }
        thread_bucket[i] = NULL;
    }
}

static sre_sptr thread_entry(sre_threadinst* inst)
{
    SDL_threadID id = SDL_ThreadID();

    const size_t index = id % SRE_THREADS_BUCKETSIZE;

    sre_threadinst* current = thread_bucket[index];
    if (!current) thread_bucket[index] = inst;
    else
    {
        while (current->next)
            current = current->next;
        current->next = inst;
    }
    
    inst->id = id;

    if (inst->delay)
        delay_s(inst->delay);
    
    assert(inst->function);

    sre_sptr res = inst->function(inst->data);
    inst->function = NULL;

    return res;
}

sre_Thread sre_threadcreate_delaystacksize(sre_sptr (*function)(void* data), void* data, sre_timeStamp delay, sre_usize stacksize)
{
    sre_threadinst* volatile inst = sre_new(sizeof(sre_threadinst));
    inst->id = 0;
    inst->next = NULL;
    inst->function = function;
    inst->data = data;
    inst->delay = delay;
    inst->handle = SDL_CreateThreadWithStackSize((SDL_ThreadFunction)thread_entry, "Story Royale Engine Thread", stacksize, inst);

    if (!inst->handle)
    {
        sre_delete(inst);
        return SRE_INVALIDTHREAD;
    }
    
    // Busy wait until id is set (it shouldn't take a lot so it's generally cheaper than suspending the thread through a system call)
    sre_Thread id = 0;
    while (!id)
        id = inst->id;
     
    return id;
}

sre_sptr sre_threadjoin(sre_Thread thrd, int* succeeded)
{
    if (!thrd) goto FAIL;

    sre_threadinst* prev = NULL;
    sre_threadinst* curr = thread_bucket[thrd % SRE_THREADS_BUCKETSIZE];
    while (curr)
    {
        if (curr->id != thrd)
        {
            prev = curr;
            curr = curr->next;
            continue;
        }
        int status;
        SDL_Thread* handle = curr->handle;

        if (prev)
            prev->next = curr->next;
        else
            thread_bucket[thrd % SRE_THREADS_BUCKETSIZE] = curr->next;

        SDL_WaitThread(handle, &status);    
        sre_delete(curr);
        return status;
    }

FAIL:
    if (succeeded) *succeeded = 0;
    return -1;
}
void sre_threaddetach(sre_Thread thrd)
{
    if (!thrd) return;

    sre_threadinst* prev = NULL;
    sre_threadinst* curr = thread_bucket[thrd % SRE_THREADS_BUCKETSIZE];
    while (curr)
    {
        if (curr->id != thrd)
        {
            prev = curr;
            curr = curr->next;
            continue;
        }
        SDL_Thread* handle = curr->handle;

        if (prev)
            prev->next = curr->next;
        else
            thread_bucket[thrd % SRE_THREADS_BUCKETSIZE] = curr->next;

        SDL_DetachThread(handle);
        return;
    }
}