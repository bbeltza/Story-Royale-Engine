#include "os_win.h"

// Direct Nt call manipulation
NTSYSCALLAPI NTSTATUS NTAPI NtDelayExecution(_In_ BOOLEAN Alertable, _In_ PLARGE_INTEGER DelayInterval);

void os_win_delay(float delay)
{
    LARGE_INTEGER time;
    time.QuadPart = (long long)(delay * -10000'000);
    NtDelayExecution(FALSE, &time);
}

void os_win_msdelay(unsigned delay)
{
    Sleep(delay);
}