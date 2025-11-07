#include "OS.h"
#include "os_win.h"

const struct os_wrapper os = {
	os_win_outputgetc,
	os_win_outputhasnline
};