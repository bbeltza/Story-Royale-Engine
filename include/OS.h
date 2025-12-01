#pragma once
#include "C/API.h"

#define funcdef(type, name, ...) type (*name)(__VA_ARGS__)

_CAPI_BEGIN

struct os_wrapper
{
	funcdef(int*, output_coordget, int buff[2]);
	funcdef(void, output_coordset, const int coords[2]);

	funcdef(int, output_getc, void);
	funcdef(int, output_hasnline, void);

	funcdef(void, delay, unsigned long long units);
};

// Operating System wrapper functions/utilities that aren't usually available in the standard library
extern const struct os_wrapper os;

_CAPI_END

#if defined(__unix)
	#include "OS/unix.h"
#elif defined(_WIN32)
	#include "OS/win.h"
#endif

#define delay_s(s) os.delay((unsigned long long)(s * TICKS_PER_SEC))
#define delay_ms(ms) os.delay(unsigned long long(s * (TICKS_PER_SEC / 1000)))