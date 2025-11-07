#include <stdio.h>

#include "logging.h"

void LOG(const char* fmt, ...)
{
	putchar('\n');
	fflush(stdout);

	int ret = getc(stdout);
}