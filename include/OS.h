#pragma once
#include "C_API.h"

#define _funcdef(type, name, ...) type (*name)(__VA_ARGS__)

SRE_CAPI_BEGIN

typedef unsigned (*os_thread_proc)(void*);

struct os_wrapper
{
	_funcdef(int*, output_coordget, int buff[2]);
	_funcdef(void, output_coordset, const int coords[2]);

	_funcdef(int, output_getc, void);
	_funcdef(int, output_hasnline, void);

	_funcdef(void, delay, unsigned long long units);
	_funcdef(unsigned long long, clock, void); // Use the most precise unit to determine the current time-stamp of the program

	_funcdef(void*, thread_spawn, os_thread_proc start, void* userdata, unsigned stacksize);
	_funcdef(int, thread_detach, void* thrd);
	_funcdef(int, thread_wait, void* thrd, unsigned* ret);

	_funcdef(int, thread_suspend, void* thrd);
	_funcdef(int, thread_resume, void* thrd);
};

#undef _funcdef

// Operating System wrapper functions/utilities that aren't usually available in the standard library
extern const struct os_wrapper os;

SRE_CAPI_END

#if defined(__unix)
	#include "OS/unix.h"
#elif defined(_WIN32)
	#include "OS/win.h"
#endif

#define delay_s(s) os.delay((unsigned long long)((s) * TICKS_PER_SEC))
#define delay_ms(ms) os.delay(unsigned long long((ms) * (TICKS_PER_SEC / 1000)))