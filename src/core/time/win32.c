#include <Windows.h>

NTSTATUS (NTAPI *pNtDelayExecution)(BOOLEAN Alertable, PLARGE_INTEGER DelayInterval);

static long long frequency()
{
    LARGE_INTEGER freq;
    HMODULE ntdll = GetModuleHandle("ntdll"); assert(ntdll != NULL);
    pNtDelayExecution = (void*)GetProcAddress(ntdll, "NtDelayExecution");
    QueryPerformanceFrequency(&freq);
    return (long long)freq.QuadPart;
}

static void ticks(long long* t_out)
{
    QueryPerformanceCounter((LARGE_INTEGER*)t_out);
}

static void wait(sre_timeStamp ts)
{
    LARGE_INTEGER ns;
    ns.QuadPart = (LONGLONG)(ts * -10000000);
    pNtDelayExecution(FALSE, &ns);
}