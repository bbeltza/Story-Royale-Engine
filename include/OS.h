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

	funcdef(void, delay, float seconds);
	funcdef(void, msdelay, unsigned milliseconds);
	funcdef(void, mindelay, unsigned long long units);
	funcdef(unsigned long long, unitspersec, void);
};

// Operating System wrapper functions/utilities that aren't usually available in the standard library
extern const struct os_wrapper os;

_CAPI_END