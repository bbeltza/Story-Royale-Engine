#include "time.h"
#include "os_unix.h"

void os_unix_delay(float delay)
{
    struct timespec duration;
    duration.tv_sec = (long)delay;
    
    delay -= duration.tv_sec;
    duration.tv_nsec = (long)(delay * 1000000000);

    nanosleep(&duration, NULL);
}

void os_unix_msdelay(unsigned delay)
{
    usleep(delay * 1000);
}