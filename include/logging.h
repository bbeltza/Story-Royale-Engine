#pragma once
#include "C/API.h"

// logging function signature type
typedef void (*logfunc_t)(const char* format, ...);

// File locking macros
#if defined(_WIN32)

#define flockfile _lock_file
#define funlockfile _unlock_file

#elif defined(__unix__)

#define flockfile flockfile
#define funlockfile funlockfile

#else

#ifndef flockfile
#define flockfile
#endif
#ifndef funlockfile
#define funlockfile
#endif

#endif

// LOG API
_CAPI_BEGIN

void NLOG(const char* format, ...);
void ALOG(const char* format, ...);

#define LOG NLOG

void DEBUG(const char* format, ...);
void ERROR(const char* format, ...);
void WARN(const char* format, ...);

_CAPI_END