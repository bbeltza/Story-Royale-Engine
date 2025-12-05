#pragma once
#include "C/API.h"

// logging function signature type
typedef void (*logfunc_t)(const char* format, ...);

// LOG API
_CAPI_BEGIN

void CUSTOM_LOG(const char* prefix, void** files, const char* fmt, void* args, int end);

void NLOG(const char* format, ...);
void ALOG(const char* format, ...);

#define LOG NLOG

void DEBUG(const char* format, ...); // Not implemented yet. I don't find much reasons to use it over LOG for now so
void ERROR(const char* format, ...);
void WARN(const char* format, ...);

extern void* const* const SRE_LOGFILE;

_CAPI_END