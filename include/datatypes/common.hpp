#pragma once
#include <utils/logging.h>

#define SRE_IMPLEMENT_DATATYPE_PRINT \
inline void print() const { display(&ALOG); } \
inline void println() const { display(&NLOG); }

#define SRE_IMPLEMENT_DATATYPE(...) SRE_IMPLEMENT_DATATYPE_PRINT \
inline void display(logfunc_t logger) const { logger(__VA_ARGS__); }