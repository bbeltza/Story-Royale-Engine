#include "Windows.h"
#include "C/API.h"

_CAPI_BEGIN

int* os_win_outputcoordget(int[2]);
void os_win_outputcoordset(const int[2]);

int os_win_outputgetc(void);
int os_win_outputhasnline(void);

void os_win_delay(float);
void os_win_msdelay(unsigned);

_CAPI_END