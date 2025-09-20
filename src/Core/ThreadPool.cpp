#include "Base/ThreadPool.h"

ThreadPool::ThreadPool()
{
    for (int i = 0; i < NUM_THREADS; i++)
    {
        threads[i] = new Thread((ThreadFunction)thread_callback, this, i);
    }
}

ThreadPool::~ThreadPool()
{
    for (int i = 0; i < NUM_THREADS; i++)
        delete threads[i];
    for (Thread *thrd : immediate_threads)
        delete thrd;

    SDL_DestroyMutex(queue_mutex);
}

void ThreadPool::thread_callback(ThreadPool *self, int index)
{
    /*
    ThreadPool* self = args->self;
    int index = args->index;
    free(args);
    */

    loop:
        SDL_LockMutex(self->queue_mutex);
        if (self->func_queue.empty())
        {
            SDL_UnlockMutex(self->queue_mutex);
            SDL_Delay(1);
            goto loop;
        }
        FuncBase back = self->func_queue.back();
        self->func_queue.pop();
        SDL_UnlockMutex(self->queue_mutex);

        back.func(
            back.args[0],
            back.args[1],
            back.args[2],
            back.args[3],
            back.args[4],
            back.args[5],
            back.args[6],
            back.args[7]);
        goto loop;
}


void ThreadPool::_queue_func(ThreadFunction func, size_t argn, ...)
{
    va_list va;
    va_start(va, argn);

    func_queue.emplace();

    FuncBase &base = func_queue.front();
    base.func = func;
    for (int i = 0; i < argn; i++)
        base.args[i] = va_arg(va, long long);
    memset(base.args + argn, 0, sizeof(long long) * (Thread::NUM_ARGS - argn));

    va_end(va);
}