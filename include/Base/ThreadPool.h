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

    template <class F, class ...arg>
    inline void Queue(F&& func, arg&&... args)
    {
        return _queue_func((ThreadFunction)func, args...);
    }

    template <class F, class ...arg>
    inline Thread& CreateImmediateThread(F&& func, arg&&... args)
    {
        if (immediate_threads.capacity() == immediate_threads.size())
            immediate_threads.reserve(NUM_THREADS);
        
        immediate_threads.push_back(new Thread((ThreadFunction)func, args...));
        return *immediate_threads.back();
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

    std::vector<Thread*> immediate_threads;

    static void thread_callback(ThreadPool* self, int index);

    void _queue_func(ThreadFunction func, ...);
};
