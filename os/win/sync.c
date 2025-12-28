#include "os_win.h"

// Direct Nt call manipulation
NTSYSCALLAPI NTSTATUS NTAPI NtDelayExecution(_In_ BOOLEAN Alertable, _In_ PLARGE_INTEGER DelayInterval);

void os_win_delay(unsigned long long us100x)
{
    LARGE_INTEGER time;
    time.QuadPart = us100x * -1;
    NtDelayExecution(TRUE, &time);
}

long long _CLOCK_FREQUENCY_REAL = 0;
const long long* const _CLOCK_FREQUENCY = &_CLOCK_FREQUENCY_REAL;

unsigned long long os_win_clock(void)
{
    LARGE_INTEGER counter;
    if (!_CLOCK_FREQUENCY_REAL)
    {
        if (!QueryPerformanceFrequency(&counter)) goto FAIL;
        _CLOCK_FREQUENCY_REAL = counter.QuadPart;
    }


    if (QueryPerformanceCounter(&counter) == 0) goto FAIL;

    return (unsigned long long)counter.QuadPart;

    FAIL:
    return 0;
}