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
    for (Thread* thrd : immediate_threads)
        delete thrd;
}

void ThreadPool::thread_callback(ThreadPool* self, int index)
{
    /*
    ThreadPool* self = args->self;
    int index = args->index;
    free(args);
    */

    while (true)
    {
        while (!SDL_LockMutex(self->queue_mutex))
        {
            if (self->func_queue.empty()) {
                SDL_UnlockMutex(self->queue_mutex);
                SDL_Delay(1);
                break;
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
                back.args[7]
            );
        }
    }
}

void ThreadPool::_queue_func(ThreadFunction func, ...)
{
    va_list va;
    va_start(va, func);

    func_queue.emplace();

    FuncBase& base = func_queue.front();
    base.func = func;
    for (int i = 0; i < 8; i++)
        base.args[i] = va_arg(va, long long);

    va_end(va);
}