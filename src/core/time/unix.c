#include <time.h>

NTSTATUS (NTAPI *pNtDelayExecution)(BOOLEAN Alertable, PLARGE_INTEGER DelayInterval);

static long long frequency()
{
    return 1000000000; 
}

static void ticks(long long* t_out)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    *t_out = ts.tv_sec * 1000000000 + ts.tv_nsec;
}

static void wait(sre_timeStamp delay)
{
    struct timespec ts;
    ts.tv_sec = delay / 1000000000;
    ts.tv_nsec = delay - ts.tv_sec * 1000000000;

    nanosleep(&ts, NULL);
}