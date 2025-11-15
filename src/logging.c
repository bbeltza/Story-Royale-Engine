#include <stdio.h>
#include <stdarg.h>

#include "OS.h"
#include "logging.h"

#define va_block(ap, x, ...) va_list ap; va_start(ap, x); __VA_ARGS__; va_end(ap)

void NLOG(const char* fmt, ...)
{
	flockfile(stdout);

	if (!os.output_hasnline())
		putchar('\n');
	fputs("[LOG]: ", stdout);

	va_block(va, fmt, vfprintf(stdout, fmt, va));

	funlockfile(stdout);
}

void ALOG(const char* fmt, ...)
{
	flockfile(stdout);
	if (os.output_hasnline())
		fputs("[LOG]: ", stdout);

	va_block(va, fmt, vfprintf(stdout, fmt, va));
	
	funlockfile(stdout);
}

void WARN(const char* fmt, ...)
{
	flockfile(stdout);
	if (!os.output_hasnline())
		putchar('\n');
	fputs("[WARNING]: ", stdout);

	va_block(va, fmt, vfprintf(stdout, fmt, va));
	
	funlockfile(stdout);
}

void ERROR(const char* fmt, ...)
{
	flockfile(stderr);
	if (!os.output_hasnline())
		putchar('\n');
	fputs("[ERROR]: ", stderr);

	va_block(va, fmt, vfprintf(stderr, fmt, va));
	fputc('\n', stderr);
	
	funlockfile(stderr);
	fflush(stderr);
}