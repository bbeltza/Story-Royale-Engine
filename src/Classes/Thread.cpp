#include "Classes/Thread.h"
#include "Engine.h"

int Thread::invokethread_handler(Thread *self)
{
    self->t_returned = self->m_func(
        self->m_args[0],
        self->m_args[1],
        self->m_args[2],
        self->m_args[3],
        self->m_args[4],
        self->m_args[5],
        self->m_args[6],
        self->m_args[7]
    );

    Engine->ThreadPool.DetachThread(*self);

    return 0;
}

Thread::Thread(Function func, ...)
{
    va_list va;
    va_start(va, func);

    m_func = func;

    for (int i = 0; i < NUM_ARGS; i++)
        m_args[i] = va_arg(va, void*);

    va_end(va);

    m_handle = SDL_CreateThread((SDL_ThreadFunction)invokethread_handler, NULL, this);
}

Thread::~Thread()
{
    SDL_DetachThread(m_handle);
}