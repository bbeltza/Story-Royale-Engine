#pragma once
#include <SDL.h>
#include <standard>

#include <Datatypes/TimeStamp.h>
#include <internal_def.hh>
#include <utils/sequence.hpp>

#include <ints.h>
#include <Base/Thread.h>

namespace sre
{
    using ThreadID = sre_Thread;
    class Thread
    {
        ThreadID m_id = SRE_INVALIDTHREAD;
    public:
        using result_type = sptr;
    private:
        template <typename Data, size_t... indices>
        static result_type invoke(void* rawdata)
        {
            auto data = static_cast<Data*>(rawdata);
            result_type ret = data->fn(std::move(std::get<indices>(data->tuple))...);
            delete data;

            return ret;
        }
        template <typename Data, size_t... indices>
        static decltype(invoke<Data, indices...>) get_invoke(ut::sequence<indices...>) { return &invoke<Data, indices...>; }

        template <typename Fn, typename Tuple>
        struct invokedata
        {
            Fn fn;
            Tuple tuple;

            template <typename... Args>
            invokedata(Fn&& func, Args &&...args) : fn(std::forward<Fn>(func)), tuple(std::forward<Args>(args)...) {}
        };
    public:
        template <typename Fn, typename... Args>
        Thread(Fn &&func, Args&&... args)
        {
            using tuple = std::tuple<typename std::decay<Args>::type...>;
            using invoker = Thread::invokedata<Fn, tuple>;

            auto invoke = new invoker(std::forward<Fn>(func), std::forward<Args>(args)...);
            auto invokefunc = get_invoke<invoker>(typename ut::make_sequence<sizeof...(Args)>::type());

            m_id = sre_threadcreate(invokefunc, invoke);
        }

        Thread() = default;

        result_type join();
        void detach();
    };
};

#define TEMPL template <class _Fn, class... _Args>
class Thread;

namespace Threads // Designed to replace ThreadPool
{
    TEMPL inline Thread Delay(sre::timeStamp Duration, _Fn &&_Fx, _Args &&..._Ax);
    TEMPL inline Thread Create(_Fn &&_Fx, _Args &&..._Ax);
}

__def_internal(__update_classes);
__def_internal(__clean_containers);

class Thread
{
    friend class ThrdPool;

public:
    typedef void (*Function)(void *);
    static const uint8_t NUM_ARGS = 8;

public:
    void Join();
    void Detach();

    ~Thread();
    Thread() {}
    Thread(Thread &&moving);

    Thread &operator=(Thread &&moving);

    struct data
    {
        ~data()
        {
            if (handle)
            {
                SDL_DetachThread(handle);
            }
            if (thrd)
                thrd->_data = nullptr;
        }
        inline bool operator==(const data &other) const { return this == &other; }
        Thread *thrd;
        SDL_Thread *handle;
        Function func;
        void *data;
        sre::timeStamp delay;
    } *_data = nullptr;

private:
    template <typename _Fx, typename _Tuple>
    struct invokedata
    {
        _Fx fn;
        _Tuple tuple;

        template <typename... _Args>
        invokedata(_Fx &&func, _Args &&...args) : fn(std::forward<_Fx>(func)), tuple(std::forward<_Args>(args)...) {}
    };

    Thread(const Thread &other) = delete;
    Thread(Function func, void *userdata, sre::timeStamp delay);

    template <typename _Data, size_t... _Indices>
    static void _invoke(void *_rawdata)
    {
        auto _data = static_cast<_Data *>(_rawdata);
        _data->fn(std::move(std::get<_Indices>(_data->tuple))...);
        delete _data;
    }

    typedef void (*_void_func)(void *);

    template <typename _Data, size_t... _Indices>
    static _void_func get_invoke(ut::sequence<_Indices...>) { return &Thread::_invoke<_Data, _Indices...>; }

    static int invokethread_handler(data *);
    static void queue_removing();

    TEMPL friend Thread Threads::Delay(sre::timeStamp Duration, _Fn &&_Fx, _Args &&..._Ax);
    __friend_internal(__update_classes);
    __friend_internal(__clean_containers);
};

TEMPL Thread Threads::Delay(sre::timeStamp Duration, _Fn &&_Fx, _Args &&..._Ax)
{
    using _Tuple = std::tuple<typename std::decay<_Args>::type...>;
    using _Invoke = Thread::invokedata<_Fn, _Tuple>;

    auto invoke = new _Invoke(std::forward<_Fn>(_Fx), std::forward<_Args>(_Ax)...);
    auto invokefunc = Thread::get_invoke<_Invoke>(typename ut::make_sequence<sizeof...(_Args)>::type());

    Thread to_move(invokefunc, invoke, Duration);
    return to_move;
}
TEMPL Thread Threads::Create(_Fn &&_Fx, _Args &&..._Ax)
{
    return Delay(
        0,
        std::forward<_Fn>(_Fx),
        std::forward<_Args>(_Ax)...);
}

#undef TEMPL