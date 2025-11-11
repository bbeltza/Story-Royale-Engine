#include "unistd.h"
#include "C/API.h"

_CAPI_BEGIN

int* os_unix_outputcoordget(int[2]);
void os_unix_outputcoordset(const int[2]);

int os_unix_outputgetc(void);
int os_unix_outputhasnline(void);

void os_unix_delay(float);
void os_unix_msdelay(unsigned);

_CAPI_END