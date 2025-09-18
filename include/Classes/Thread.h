#pragma once
#include <SDL.hpp>

class Thread
{
    friend class ThreadPool;
public:
    typedef void (*Function)(...);
    static const uint8_t NUM_ARGS = 8;  
public:
    /* Ready for some public methods */

private:
    Thread(Function func, ...);
    ~Thread();
    SDL_Thread *m_handle;

    Function m_func;
    long long m_args[NUM_ARGS];

    static int invokethread_handler(Thread *self);
};
