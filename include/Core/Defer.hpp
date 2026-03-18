#ifndef SRE_DEFER_HPP
#define SRE_DEFER_HPP
// Header defer utilities with C++ support
// For now, it's just a wrapper for sre_defer that supports distinct types of pointers
// But more stuff might be on the way

#include <Core/Defer.h>
#include <utils/sequence.hpp>
namespace sre
{
    template <class T>
    void defer(void (*func)(T* data), T* data) { return sre_defer(reinterpret_cast<sre_deferFunction>(func), data); }

    template <typename Fn, typename... Args>
    struct _DeferWrapper
    {
        using Tuple = std::tuple<Args...>;

        Fn&& func;
        Tuple tuple;

        template <size_t... indices>
        static void call(void* _wrapper)
        {
            std::unique_ptr<_DeferWrapper> _this(static_cast<_DeferWrapper*>(_wrapper));
            _this->func(std::get<indices>(_this->tuple)...);
        }

        template <size_t... indices>
        static sre_deferFunction _getcall(ut::sequence<indices...>) { return &call<indices...>; }
    };
    // defer multiarg wrapper (uses additional memory...)
    template <class Fn, typename... Args>
    void defer(Fn&& func, Args... args)
    {
        using Wrapper = _DeferWrapper<Fn&&, Args...>;
        using Tuple = typename Wrapper::Tuple;

        auto wrapper = new Wrapper{ std::forward<Fn>(func), Tuple{args...} };
        sre_defer( Wrapper::_getcall(typename ut::make_sequence<sizeof...(Args)>::type{}), wrapper);
    }
}

#endif