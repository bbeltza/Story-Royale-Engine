#include <stdio.h>

#include "OS.h"
#include "logging.h"

void NLOG(const char* fmt, ...)
{
	if (!os.output_hasnline())
		putchar('\n');

	fputs("[LOG]: ", stdout);
	printf(fmt);
}