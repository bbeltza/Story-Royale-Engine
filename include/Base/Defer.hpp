#ifndef SRE_DEFER_HPP
#define SRE_DEFER_HPP
// Header defer utilities with C++ support
// For now, it's just a wrapper for sre_defer that supports distinct types of pointers
// But more stuff might be on the way

#include <Base/Defer.h>
namespace sre
{
    template <class T>
    void defer(void (*func)(T* data), T* data) { return sre_defer(reinterpret_cast<sre_deferFunction>(func), data); }
}

#endif