#ifndef OS_WIN_H
#define OS_WIN_H

#include <Windows.h>

#include <OS.h>
#include <OS/win.h>

SRE_CAPI_BEGIN

int* os_win_outputcoordget(int[2]);
void os_win_outputcoordset(const int[2]);

int os_win_outputgetc(void);
int os_win_outputhasnline(void);

void os_win_delay(unsigned long long units);
unsigned long long os_win_clock(void);

void* os_win_thread_spawn(os_thread_proc start, void* userdata, unsigned stacksize);
int os_win_thread_detach(void* thrd);
int os_win_thread_wait(void* thrd, unsigned* ret);

SRE_CAPI_END

#endif