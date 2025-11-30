#include "os_win.h"

// Direct Nt call manipulation
NTSYSCALLAPI NTSTATUS NTAPI NtDelayExecution(_In_ BOOLEAN Alertable, _In_ PLARGE_INTEGER DelayInterval);

void os_win_delay(unsigned long long us100x)
{
    LARGE_INTEGER time;
    time.QuadPart = us100x * -1;
    NtDelayExecution(TRUE, &time);
}