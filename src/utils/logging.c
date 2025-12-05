#define va_block(ap, x, ...) va_list ap; va_start(ap, x); __VA_ARGS__; va_end(ap)
#include "utils/logging.h"
#include "utils/lockfile.h"

#ifndef ANDROID
#include "OS.h"

#define PREFIX(x) "[" #x "]: "

void CUSTOM_LOG(const char* prefix, FILE** files, const char* fmt, va_list args, int end)
{
	flockfile(stdout); // Only need to lock stdout as it would wait anyways for every file to print

	int hasnline = os.output_hasnline();
	for (size_t i = 0; files[i]; i++)
	{
		register FILE* file = files[i];
		if (end == '\1')
		{
			if (!hasnline)
				fputc('\n', file);
			fputs(prefix, file);
			if (args)
				vfprintf(file, fmt, args);
			else
				fputs(fmt, file);
			putc('\n', file);
		}
		else
		{
			if (hasnline)
				fputs(prefix, file);
			if (args)
				vfprintf(file, fmt, args);
			else
				fputs(fmt, file);
			putc(end, file);
		}
	}

	funlockfile(stdout);
}

void NLOG(const char* fmt, ...)
{
	FILE* files[] = {
		stdout,
		*SRE_LOGFILE,
		NULL
	};

	va_block(va, fmt, CUSTOM_LOG(PREFIX(LOG), files, fmt, va, '\n'));
}

void ALOG(const char* fmt, ...)
{
	FILE* files[] = {
		stdout,
		*SRE_LOGFILE,
		NULL
	};

	va_block(va, fmt, CUSTOM_LOG(PREFIX(LOG), files, fmt, va, '\0'));
}

#define WARN_NEWLINE_CHECK if (!os.output_hasnline()) fputc('\n', stderr)

void WARN(const char* fmt, ...)
{
	FILE* files[] = {
		stderr,
		*SRE_LOGFILE,
		NULL
	};

	va_block(va, fmt, CUSTOM_LOG(PREFIX(WARN), files, fmt, va, '\1'));
}

void ERROR(const char* fmt, ...)
{
	FILE* files[] = {
		stderr,
		*SRE_LOGFILE,
		NULL
	};

	va_block(va, fmt, CUSTOM_LOG(PREFIX(ERROR), files, fmt, va, '\1'));
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