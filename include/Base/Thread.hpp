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
            std::unique_ptr<Data> data(static_cast<Data*>(rawdata));

            return data->fn(std::move(std::get<indices>(data->tuple))...);
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
            static_assert(!std::is_void<decltype(func(std::forward<Args>(args)...))>::value, "Fn &&func must return a value");

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