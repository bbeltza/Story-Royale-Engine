#include "Classes/Thread.hpp"
#include "../internal.h"

static std::list<std::unique_ptr<Thread>> threads_list;

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

    threads_list.emplace_back(this);
}

Thread::~Thread()
{
    SDL_DetachThread(m_handle);
}

void Thread::Join()
{
    SDL_WaitThread(m_handle, NULL);
    Detach();
}

void Thread::Detach()
{
    threads_list.remove_if([&](const std::unique_ptr<Thread>& _Other) -> bool { return this == _Other.get(); });
}