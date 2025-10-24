#pragma once
#include <SDL.hpp>
#include <standard>
#define TEMPL template <class _Fn, class... _Args>

#include "Datatypes/TimeStamp.h"

#include "internal_def.hh"

#include "CppUtils/sequence.hpp"

class Thread;

namespace Threads // Designed to replace ThreadPool
{
    TEMPL Thread Create(_Fn&& _Fx, _Args&&... _Ax) {
        return Delay(
        0,
        std::forward<_Fn>(_Fx),
        std::forward<_Args>(_Ax)...); }
    TEMPL Thread Delay(TimeStamp Duration, _Fn&& _Fx, _Args&&... _Ax);
}

__def_internal(__update_classes)

class Thread
{
    friend class ThrdPool;
public:

typedef void* (*Function)(void*);
static const uint8_t NUM_ARGS = 8;  
public:
    void Join();
    void Detach();

    ~Thread();
    Thread() {}
    Thread(Thread&& moving);

    Thread& operator=(Thread&& moving);

    struct data
    {
        ~data();
        inline bool operator==(const data& other) const { return this == &other; }
        Thread* thrd;
        SDL_Thread* handle;
        Function func;
        void* data;
        TimeStamp delay;
    } *_data = nullptr;
private:
    Thread(const Thread& other) = delete;
    Thread(Function func, void* userdata, TimeStamp delay);

    template <typename _Tuple, size_t... _Indices>
    static Function get_invoke(ut::sequence<_Indices...>) { return (Function)&_invoke<_Tuple, _Indices...>; }
    template <typename _Tuple, size_t... _Indices>
    static void _invoke(void* _rawdata)
    {
        auto _data = static_cast<_Tuple*>(_rawdata);
        std::invoke(std::move(std::get<_Indices>(*_data))...);
        delete _data;
    }

    static int invokethread_handler(data *);
    static void queue_removing();
    static std::list<data> threads_list;
    static std::queue<data*> to_remove;

    TEMPL friend Thread Threads::Delay(TimeStamp Duration, _Fn&& _Fx, _Args&&... _Ax);
    __friend_internal(__update_classes)
};

TEMPL Thread Threads::Delay(TimeStamp Duration, _Fn&& _Fx, _Args&&... _Ax)
{
    using _Tuple = std::tuple<std::decay_t<_Fn>, std::decay_t<_Args>...>;

    auto tuple = new _Tuple(std::forward<_Fn>(_Fx), std::forward<_Args>(_Ax)...);
    auto invokefunc = Thread::get_invoke<_Tuple>(typename ut::make_sequence<1 + sizeof...(_Args)>::type());

    return Thread(invokefunc, tuple, Duration);
}

#undef TEMPL