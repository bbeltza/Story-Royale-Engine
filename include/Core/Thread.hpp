#ifndef SRE_THREAD_HPP
#define SRE_THREAD_HPP
#include <Core/Thread.h>
#include <utils/sequence.hpp>
#include <standard>

namespace sre
{
    using ThreadID = sre_Thread;
    class Thread
    {
        ThreadID m_id = SRE_INVALIDTHREAD;
    public:
        using result_type = sptr;
    private:
        template <typename Ret, typename Cls, typename... Args>
        static typename std::result_of<Ret (Cls::*)(Args&&...)>::type function_invoke(Ret (Cls::*&&fn)(Args&&...), Cls*&& cls, Args&&... args)
        {
            return (cls->*fn)(std::forward<Args>(args)...);
        }

        template <typename Fn, typename... Args>
        static typename std::result_of<Fn(Args&&...)>::type function_invoke(Fn&& fn, Args&&... args)
        {
            return fn(std::forward<Args>(args)...);
        }

        template <typename Data, size_t... indices>
        static result_type invoke(void* rawdata)
        {
            std::unique_ptr<Data> data(static_cast<Data*>(rawdata));

            return function_invoke(data->fn, std::move(std::get<indices>(data->tuple))...);
        }
        template <typename Data, size_t... indices>
        static decltype(&invoke<Data, indices...>) get_invoke(ut::sequence<indices...>) { return &invoke<Data, indices...>; }

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
            using func_restype = typename std::result_of<Fn(Args&&...)>::type;
            static_assert(!std::is_void<func_restype>::value, "Fn &&func must return a value");

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

#endif