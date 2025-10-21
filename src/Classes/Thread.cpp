#include "Classes/Thread.hpp"
#include "../internal.h"

#include "Sys.h"

std::list<Thread::data> Thread::threads_list;
std::queue<Thread::data*> Thread::to_remove;

void Thread::queue_removing()
{
    while (!to_remove.empty())
    {
        auto thrd_data = to_remove.front();
        SDL_WaitThread(thrd_data->handle, NULL);
        thrd_data->handle = nullptr;
        threads_list.remove(*thrd_data);
        to_remove.pop();
    }
}

int Thread::invokethread_handler(data *_data)
{
    _data->func(
        _data->args[0],
        _data->args[1],
        _data->args[2],
        _data->args[3],
        _data->args[4],
        _data->args[5],
        _data->args[6],
        _data->args[7]
    );

    to_remove.push(_data);

    return 0;
}

Thread::Thread(Function func, ...)
{
    threads_list.emplace_back();
    _data = &threads_list.back();
    _data->func = func;
    _data->thrd = this;

    va_list va;
    va_start(va, func);

    for (int i = 0; i < NUM_ARGS; i++)
        _data->args[i] = va_arg(va, void*);

    va_end(va);

    _data->handle = SDL_CreateThread((SDL_ThreadFunction)invokethread_handler, NULL, _data);
}

Thread::Thread(Thread&& moving) : _data(moving._data) { moving._data = nullptr; _data->thrd = this; }

Thread::~Thread() { if (_data) _data->thrd = nullptr; }

Thread::data::~data()
{
    if (handle)
        SDL_DetachThread(handle);
    if (thrd)
        thrd->_data = nullptr;
}

void Thread::Join()
{
    assert(_data);
    assert(_data->handle);
    SDL_WaitThread(_data->handle, NULL);
}

void Thread::Detach()
{
    assert(_data);
    threads_list.remove(*_data);
}