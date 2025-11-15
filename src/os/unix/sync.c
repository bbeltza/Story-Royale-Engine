#include <time.h>

#include "os_unix.h"

void os_unix_delay(unsigned long long delay)
{
    struct timespec ts;
    ts.tv_sec = delay / TICKS_PER_SEC;
    ts.tv_nsec = delay - ts.tv_sec * TICKS_PER_SEC;

    nanosleep(&ts, NULL);
}