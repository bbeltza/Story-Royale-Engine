#include <time.h>

static long long frequency()
{
    return 1000000000LL; 
}

static void ticks(long long* t_out)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long long t = ts.tv_sec * 1000000000LL + ts.tv_nsec;
    assert(t > 0);
    *t_out = t;
}

static void wait(sre_timeStamp delay)
{
    struct timespec ts;
    ts.tv_sec = delay;
    ts.tv_nsec = (delay - ts.tv_sec) * 1000000000;

    nanosleep(&ts, NULL);
}