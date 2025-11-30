#include <Windows.h>

#include "OS/win.h"
#include "C/API.h"

_CAPI_BEGIN

int* os_win_outputcoordget(int[2]);
void os_win_outputcoordset(const int[2]);

int os_win_outputgetc(void);
int os_win_outputhasnline(void);

void os_win_delay(unsigned long long);

_CAPI_END