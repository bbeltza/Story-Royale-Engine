#pragma once

#include <standard>
#include <SDL.hpp>
#include "EngineBase.hpp"
#include "Classes/Thread.hpp"

#include <assert.h>

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
        assert("Attempt to detach an invalid thread. Thread has probably already been detached by the ThreadPool destructor" && immediate_threads.count(&thrd));
        immediate_threads.erase(&thrd);
        delete &thrd;
    }

    template <class ret>
    inline ret JoinThread(Thread& thrd)
    {
        SDL_WaitThread(thrd.m_handle, NULL);
        void* retval = thrd.t_returned;
        immediate_threads.erase(&thrd);
        return (ret)retval;
    }
    inline void* JoinThread(Thread& thrd) { return JoinThread<void*>(thrd); }

    private:
    struct FuncBase
    {
        ThreadFunction func;
        void* args[Thread::NUM_ARGS];
    };
    
    std::unordered_set<Thread*> immediate_threads;

    void detach_threads();
};
