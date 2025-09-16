#include "Base/ThreadPool.h"

ThreadPool::ThreadPool()
{
    for (int i = 0; i < NUM_THREADS; i++)
    {
        CallbackArgs* args = new CallbackArgs;
        args->self = this;
        args->index = i;
        threads[i] = SDL_CreateThread((SDL_ThreadFunction)thread_callback, NULL, args);
    }

}

ThreadPool::~ThreadPool()
{
    for (int i = 0; i < NUM_THREADS; i++)
        SDL_DetachThread(threads[i]);
}

int ThreadPool::thread_callback(CallbackArgs* args)
{
    ThreadPool* self = args->self;
    int index = args->index;
    free(args);

    printf("self: %p\n", self);

    while (true)
    {
        while (!SDL_LockMutex(self->queue_mutex))
        {;
            if (self->func_queue.empty()) {
                SDL_UnlockMutex(self->queue_mutex);
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
        SDL_Delay(1);
    }

    return 0;
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