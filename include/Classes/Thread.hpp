#pragma once
#include <SDL.hpp>
#include <standard>
#define TEMPL template <class F, class... _args>

class Thread;

namespace Threads // Designed to replace ThreadPool
{
    TEMPL Thread& Create(F&& func, _args&&... args);
}

class Thread
{
    friend class ThrdPool;
public:

typedef void* (*Function)(...);
static const uint8_t NUM_ARGS = 8;  
public:
    void Join();
    void Detach();
private:
    Thread(Function func, ...);
    ~Thread();

    friend struct std::default_delete<Thread>;
    TEMPL friend Thread& Threads::Create(F&& func, _args&&... args);

    SDL_Thread *m_handle;

    Function m_func;
    void* m_args[NUM_ARGS];

    void* t_returned;

    static int invokethread_handler(Thread *self);
};

TEMPL Thread& Threads::Create(F&& func, _args&&... args) { return *new Thread((Thread::Function)func, args...); }

#undef TEMPL