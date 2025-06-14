#ifndef SYSTEM_H
#define SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdarg.h>

    void _system_err(unsigned short LINE, const char* FILE, const char* fmt, ...);
    
#define sys_error(fmt, ...) _system_err(__LINE__, __FILE__, fmt, __VA_ARGS__)
#define sys_assert(expr, fmsg, ...) if (!expr) sys_error(fmsg, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // SYSTEM_H