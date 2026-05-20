#ifndef SRE_DEFER_H
#define SRE_DEFER_H
#include <C_API.h>
#include <ints.h>

SRE_CAPI_BEGIN

typedef void (*sre_deferFunction)(void* args);
typedef intptr_t (*sre_deferresFunction)(void* args);

// Push a function to the main thread's event handler
// This is pretty useful for multithreading, since there are features that aren't fully supported on multiple threads
// (Like creating a texture), or to manage synchronisation problems
// `argsize` specifies the size in bytes to copy inside `arg`. If `argsize` is higher than the size of a pointer,
//      then `arg` will be allocated to fit `argsize`. Otherwise, the value will be directly copied into the event structure.
//      This also means that whatever is passed in `func` won't be the same value as `arg` in the function call.
// Unless `argsize` is 0, in this case, `arg` will be taken as a value.
// Or if `arg` is NULL, then `argsize` is ignore and `func` will recieve NULL in the argument.
// However, you cannot use any coroutine yielding functions such as sre_coroutineyield
bool sre_defer(sre_deferFunction func, size_t argsize, const void* arg);
intptr_t sre_defer_res(sre_deferresFunction func, void* arg);

SRE_CAPI_END

#ifdef __cplusplus

#include <utils/sequence.hpp>
#include <memory>
namespace sre
{
    inline bool defer(sre_deferFunction func, size_t argsize, const void* arg) { return sre_defer(func, argsize, arg); }

    template <class T>
    bool defer(void (*func)(T* data), T* data) { return sre_defer(reinterpret_cast<sre_deferFunction>(func), 0, data); }

    template <typename Fn, typename... Args>
    struct _DeferWrapper
    {
        using Tuple = std::tuple<Args...>;

        Fn&& func;
        Tuple tuple;

        template <size_t... indices>
        static void call(void* _arg)
        {
            auto _this = static_cast<_DeferWrapper*>(_arg);
            _this->func(std::get<indices>(_this->tuple)...);
        }

        template <size_t... indices>
        static sre_deferFunction _getcall(ut::sequence<indices...>) { return &call<indices...>; }
    };
    // defer multiarg wrapper (uses additional memory...)
    template <class Fn, typename... Args>
    bool defer(Fn&& func, Args... args)
    {
        using Wrapper = _DeferWrapper<Fn&&, Args...>;
        using Tuple = typename Wrapper::Tuple;

        Wrapper wrapper{std::forward<Fn>(func), Tuple{args...}};
        return sre_defer( Wrapper::_getcall(typename ut::make_sequence<sizeof...(Args)>::type{}), sizeof(Wrapper), &wrapper);
    }
}

#endif

#endif