#include "OS.h"
#include "os_unix.h"

const struct os_wrapper os = {
	os_unix_outputgetc,
	os_unix_outputhasnline
};