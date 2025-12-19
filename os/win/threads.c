#include "os_win.h"

void* os_win_thread_spawn(os_thread_proc start, void* userdata, unsigned stacksize)
{
	return CreateThread(NULL, stacksize, (LPTHREAD_START_ROUTINE)start, userdata, 0, NULL);
}

int os_win_thread_detach(void* thrd)
{
	if (CloseHandle((HANDLE)thrd))
		return 0;
	else
		return -1;
}

int os_win_thread_wait(void* thrd, unsigned* ret)
{
	WaitForSingleObject((HANDLE)thrd, INFINITE);
	GetExitCodeThread((HANDLE)thrd, ret);
	CloseHandle((HANDLE)thrd);
}