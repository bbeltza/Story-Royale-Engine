#include "OS.h"
#include "os_unix.h"

const struct os_wrapper os = {
	os_unix_outputcoordget,
	os_unix_outputcoordset,

	os_unix_outputgetc,
	os_unix_outputhasnline,

	os_unix_delay,
	os_unix_msdelay
};