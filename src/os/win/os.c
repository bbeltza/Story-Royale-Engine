#include "OS.h"
#include "os_win.h"

const struct os_wrapper os = {
	os_win_outputcoordget,
	os_win_outputcoordset,

	os_win_outputgetc,
	os_win_outputhasnline,
	
	os_win_delay,
	os_win_msdelay
};