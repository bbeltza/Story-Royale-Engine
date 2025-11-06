#include <stdio.h>
#include "Sys.h"

#define lockfile _lock_file
#define unlockfile _unlock_file

int sysvlog(const char* fmt, va_list va)
{
    lockfile(stdout);
    fputs("[LOG]: ", stdout);
    int c = vfprintf(stdout, fmt, va);
    unlockfile(stdout);
    return c;
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