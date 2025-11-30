#define va_block(ap, x, ...) va_list ap; va_start(ap, x); __VA_ARGS__; va_end(ap)
#include <stdarg.h>

#include "utils/logging.h"
#include "utils/lockfile.h"

#ifndef ANDROID
#include <stdio.h>
#include "OS.h"

void NLOG(const char* fmt, ...)
{
	flockfile(stdout);
	if (os.output_hasnline())
		fputs("[LOG]: ", stdout);

	va_block(va, fmt, vfprintf(stdout, fmt, va));
	putc('\n', stdout);

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

#define WARN_NEWLINE_CHECK if (!os.output_hasnline()) fputc('\n', stderr)

void WARN(const char* fmt, ...)
{
	flockfile(stderr);
	
	WARN_NEWLINE_CHECK;

	fputs("[WARNING]: ", stderr);

	va_block(va, fmt, vfprintf(stderr, fmt, va));
	fputc('\n', stderr);
	
	funlockfile(stderr);
}

void ERROR(const char* fmt, ...)
{
	flockfile(stderr);
	
	WARN_NEWLINE_CHECK;

	fputs("[ERROR]: ", stderr);

	va_block(va, fmt, vfprintf(stderr, fmt, va));
	fputc('\n', stderr);
	
	funlockfile(stderr);
	fflush(stderr);
}
#else
#include <android/log.h>

#define DEFINE_LOG(p, t, n) void n(const char* fmt, ...) { va_block(va, fmt, __android_log_vprint(p, "GAME " t, fmt, va)); }

DEFINE_LOG(ANDROID_LOG_INFO, "LOG", NLOG)
DEFINE_LOG(ANDROID_LOG_INFO, "LOG", ALOG)
DEFINE_LOG(ANDROID_LOG_DEBUG, "DEBUG", DEBUG)
DEFINE_LOG(ANDROID_LOG_ERROR, "ERROR", ERROR)
DEFINE_LOG(ANDROID_LOG_WARN, "WARNING", WARN)

#endif