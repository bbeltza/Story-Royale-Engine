#pragma once
#include "C/API.h"

_CAPI_BEGIN

void NLOG(const char* fmt, ...);
void ALOG(const char* fmt, ...);

#define LOG NLOG

void ERROR(const char* fmt, int level, ...);
void WARN(const char* fmt, ...);

_CAPI_END