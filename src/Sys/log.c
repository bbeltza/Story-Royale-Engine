#include <stdio.h>
#include <stdarg.h>
#include "Sys.h"

int sysvlog(const char* fmt, va_list va)
{
    fputs("[LOG]: ", stdout);
    return vprintf(fmt, va);
}

int syslog(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    int c = sysvlog(fmt, va);

    va_end(va);

    return c;
}

int syslogln(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    int c = sysvlog(fmt, va);
    putchar('\n');

    va_end(va);

    return c;
}