#include "Base/Thread.hpp"
#include "../internal.h"

#include "OS.h"
#include "logging.h"

#define threads_list reinterpret_cast<std::list<Thread::data>*>(engine.allocated_threads)
#define to_remove reinterpret_cast<std::queue<Thread::data*>*>(engine.finished_threads)

void Thread::queue_removing()
{
    while (!to_remove->empty())
    {
        auto thrd_data = to_remove->front();
        SDL_WaitThread(thrd_data->handle, NULL);
        thrd_data->handle = nullptr;
        threads_list->remove(*thrd_data);
        to_remove->pop();
    }
}

int Thread::invokethread_handler(data *_data)
{
    if (_data->delay) os.delay(_data->delay);

    _data->func(_data->data);
    
    if (engine.finished_threads)
        to_remove->push(_data);

    return 0;
}

Thread::Thread(Function func, void* userdata, TimeStamp delay)
{
    threads_list->emplace_back();
    _data = &threads_list->back();
    _data->func = func;
    _data->thrd = this;
    _data->data = userdata;
    _data->delay = delay;

    _data->handle = SDL_CreateThread((SDL_ThreadFunction)invokethread_handler, "Story Royale Engine Thread", _data);
}

Thread::Thread(Thread&& moving) : _data(moving._data) { moving._data = nullptr; _data->thrd = this; }
Thread& Thread::operator=(Thread&& moving) { _data = moving._data; _data->thrd = this; moving._data = nullptr; return *this; }

Thread::~Thread() { if (_data) _data->thrd = nullptr; }

Thread::data::~data()
{
    if (handle)
    {
        SDL_DetachThread(handle);
    }
    if (thrd)
        thrd->_data = nullptr;
}

void Thread::Join()
{
    if (_data && _data->handle)
    {
        SDL_WaitThread(_data->handle, NULL);
        _data->handle = nullptr;
    }
}

void Thread::Detach()
{
    if (_data && engine.allocated_threads)
        threads_list->remove(*_data);
}