#pragma once
#include <standard>
#include <C_API.h>

// logging function signature type
typedef void (*logfunc_t)(const char* format, ...);

// LOG API
SRE_CAPI_BEGIN

void CUSTOM_LOG(const char* prefix, FILE** files, const char* fmt, va_list args, int end);

void NLOG(const char* format, ...);
void ALOG(const char* format, ...);

#define LOG NLOG

void DEBUG(const char* format, ...); // Not implemented yet. I don't find much reasons to use it over LOG for now so
void ERROR(const char* format, ...);
void WARN(const char* format, ...);

extern FILE* const* const SRE_LOGFILE;

SRE_CAPI_END