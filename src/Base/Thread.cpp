#include "Base/Thread.hpp"
#include "../internal.h"
#include "../internal.hpp"

#include "OS.h"
#include "utils/logging.h"

void Thread::queue_removing()
{
    _containers->lock();

    while (!_containers->finished_threads.empty())
    {
        auto thrd_data = _containers->finished_threads.front();
        SDL_WaitThread(thrd_data->handle, NULL);
        thrd_data->handle = nullptr;
        _containers->allocated_threads.remove(*thrd_data);
        _containers->finished_threads.pop();
    }

    _containers->unlock();
}

int Thread::invokethread_handler(data *_data)
{
    if (_data->delay) delay_s(_data->delay);

    _data->func(_data->data);

    if (!engine.containers_service) return 1;
    _containers->lock();
    _containers->finished_threads.push(_data);
    _containers->unlock();

    return 0;
}

Thread::Thread(Function func, void* userdata, TimeStamp delay)
{
    _containers->lock();
    _containers->allocated_threads.emplace_back();
    _data = &_containers->allocated_threads.back();
    _data->func = func;
    _data->thrd = this;
    _data->data = userdata;
    _data->delay = delay;

    _data->handle = SDL_CreateThread((SDL_ThreadFunction)invokethread_handler, "Story Royale Engine Thread", _data);
    _containers->unlock();
}

Thread::Thread(Thread&& moving) : _data(moving._data) { moving._data = nullptr; _data->thrd = this; }
Thread& Thread::operator=(Thread&& moving) { _data = moving._data; _data->thrd = this; moving._data = nullptr; return *this; }

Thread::~Thread() { if (_data) _data->thrd = nullptr; }

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
    if (!engine.containers_service || !_data) return;

    _containers->lock();
    _containers->allocated_threads.remove(*_data);
    _containers->unlock();
}