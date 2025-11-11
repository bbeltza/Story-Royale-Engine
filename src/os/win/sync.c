#include "os_win.h"

#define UNITS_PER_SEC 10'000'000

// Direct Nt call manipulation
NTSYSCALLAPI NTSTATUS NTAPI NtDelayExecution(_In_ BOOLEAN Alertable, _In_ PLARGE_INTEGER DelayInterval);


void os_win_mindelay(unsigned long long us100x)
{
    LARGE_INTEGER time;
    time.QuadPart = us100x * -1;
    NtDelayExecution(TRUE, &time);
}

void os_win_delay(float delay)
{
    os_win_mindelay((unsigned long long)(delay * UNITS_PER_SEC));
}

void os_win_msdelay(unsigned delay)
{
    Sleep(delay);
}