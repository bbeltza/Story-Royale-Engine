#include "Windows.h"
#include "C/API.h"

_CAPI_BEGIN

short* os_win_outputcoordget(short[2]);
void os_win_outputcoordset(const short[2]);

int os_win_outputgetc(void);
int os_win_outputhasnline(void);

void os_win_delay(float);
void os_win_msdelay(unsigned);

_CAPI_END