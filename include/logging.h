#pragma once
#include "C/API.h"

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

void NLOG(const char* fmt, ...);
void ALOG(const char* fmt, ...);

#define LOG NLOG

void DEBUG(const char* fmt, ...);
void ERROR(const char* fmt, int level, ...);
void WARN(const char* fmt, ...);

_CAPI_END