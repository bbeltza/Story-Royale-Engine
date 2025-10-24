#pragma once
#include <SDL.hpp>
#include <standard>
#define TEMPL template <class F, class... _args>

#include "internal_def.hh"

#include "CppUtils/sequence.hpp"

class Thread;

template <typename F, typename _tupl>
struct __threaduserdata
{
    template <typename... _args> __threaduserdata(F&& func, _args&&... args) : f(func), tuple(args...) {}
    F&& f;
    _tupl tuple;
};

namespace Threads // Designed to replace ThreadPool
{
    TEMPL Thread Create(F&& func, _args&&... args);
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
    } *_data = nullptr;
private:
    Thread(const Thread& other) = delete;
    Thread(Function func, void* userdata);

    template <typename F, typename _tupl, size_t... _sizes >
    static Function get_invoke(ut::sequence<_sizes...>) { return (Function)&_invoke<F, _tupl, _sizes...>; }
    template <typename F, typename _tupl, size_t... _sizes>
    static void _invoke(void* _rawdata)
    {
        syslogln("%p", _rawdata);
        auto _data = reinterpret_cast<__threaduserdata<F, _tupl>*>(_rawdata);
        _data->f(std::move(std::get<_sizes>(_data->tuple))...);
        delete _data;
    }

    static int invokethread_handler(data *);
    static void queue_removing();
    static std::list<data> threads_list;
    static std::queue<data*> to_remove;

    TEMPL friend Thread Threads::Create(F&& func, _args&&... args);
    __friend_internal(__update_classes)
};

TEMPL Thread Threads::Create(F&& func, _args&&... args)
{
    using _tupl = std::tuple<_args...>;

    auto data = new __threaduserdata<F, _tupl>(func, args...);
    syslogln("%p %p", func, data);
    auto invokefunc = Thread::get_invoke<F, _tupl>(typename ut::make_sequence<sizeof...(_args)>::type());

    return Thread(invokefunc, data);
}

#undef TEMPL