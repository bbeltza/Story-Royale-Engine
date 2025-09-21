#pragma once

#include <standard.h>
#include <SDL.hpp>
#include "EngineBase.h"
#include "Classes/Thread.h"

class ThrdPool
{
    ENGINE_BASE
    ThrdPool(EngineClass*);
    ~ThrdPool();
    public:
        typedef Thread::Function ThreadFunction;
    public:

    template <class F, class ...arg>
    inline Thread& CreateImmediateThread(F&& func, arg&&... args)
    {
        Thread* thrd = new Thread((ThreadFunction)func, args...);
        immediate_threads.insert(thrd);
        return *thrd;
    }

    inline void DetachThread(Thread& thrd)
    {
        immediate_threads.erase(&thrd);
        delete &thrd;
    }

    private:
    struct FuncBase
    {
        ThreadFunction func;
        void* args[Thread::NUM_ARGS];
    };
    
    std::unordered_set<Thread*> immediate_threads;

    void detach_threads();
};
