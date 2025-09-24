#include <stdio.h>
#include <stdarg.h>
#include "Sys.h"

int syslog(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    int c = vprintf(fmt, va);

    va_end(va);

    return c;
}

int syslogln(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    int c = syslog(fmt, va);
    putchar('\n');

    va_end(va);

    return c;
}