#pragma once
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif
int sysvlog(const char* fmt, va_list va);
int syslog(const char* fmt, ...);
int syslogln(const char* fmt, ...);
#ifdef __cplusplus
}
#endif