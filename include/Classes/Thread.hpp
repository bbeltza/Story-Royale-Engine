#pragma once
#include <SDL.hpp>
#include <standard>

class Thread
{
    friend class ThrdPool;
public:

typedef void* (*Function)(...);
static const uint8_t NUM_ARGS = 8;  
public:
/* Ready for some public methods */
private:
    Thread(Function func, ...);
    ~Thread();

    SDL_Thread *m_handle;

    Function m_func;
    void* m_args[NUM_ARGS];

    void* t_returned;

    static int invokethread_handler(Thread *self);
};
