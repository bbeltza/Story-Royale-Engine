#ifndef OS_UNIX_H
#define OS_UNIX_H

#include <unistd.h>

#include "OS/unix.h"
#include "C_API.h"

SRE_CAPI_BEGIN

int* os_unix_outputcoordget(int[2]);
void os_unix_outputcoordset(const int[2]);

int os_unix_outputgetc(void);
int os_unix_outputhasnline(void);

void os_unix_delay(unsigned long long delay);
unsigned long long os_unix_clock(void);

SRE_CAPI_END

#endif