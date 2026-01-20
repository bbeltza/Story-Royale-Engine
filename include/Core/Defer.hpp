#ifndef SRE_DEFER_HPP
#define SRE_DEFER_HPP
// Header defer utilities with C++ support
// For now, it's just a wrapper for sre_defer that supports distinct types of pointers
// But more stuff might be on the way

#include <Core/Defer.h>
namespace sre
{
    template <class T>
    void defer(void (*func)(T* data), T* data) { return sre_defer(reinterpret_cast<sre_deferFunction>(func), data); }

    // defer lambda wrapper (uses additional memory...)
    template <class Fn, typename T>
    void defer(Fn&& func, T* data)
    {
        struct Wrapper
        {
            Fn func;
            T* data;

            static void call(void* _wrapper)
            {
                std::unique_ptr<Wrapper> _this(static_cast<Wrapper*>(_wrapper));
                _this->func(_this->data);
            }
        };

        auto wrapper = new Wrapper{ func, data };
        sre_defer(Wrapper::call, wrapper);
    }
}

#endif