#pragma once

#include <standard.h>
#include <SDL.hpp>
#include "EngineBase.h"
#include "Classes/Thread.h"

class ThreadPool
{
    ENGINE_BASE
    public:
        typedef Thread::Function ThreadFunction;
    public:
    ThreadPool();
    ~ThreadPool();

    template <class retType, class ...arg>
    inline void Queue(retType(*func)(arg...), arg... args)
    {
        return _queue_func((ThreadFunction)func, args...);
    }

    static const size_t NUM_THREADS = 12;



    private:
    struct FuncBase
    {
        ThreadFunction func;
        long long args[Thread::NUM_ARGS];
    };

    SDL_mutex* queue_mutex = SDL_CreateMutex();
    Thread* threads[NUM_THREADS];
    std::queue<FuncBase> func_queue;

    static int thread_callback(ThreadPool* self, int index);

    void _queue_func(ThreadFunction func, ...);
};