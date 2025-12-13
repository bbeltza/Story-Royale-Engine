#pragma once
#include <utils/logging.h>

#define SRE_IMPLEMENT_DATATYPE(...) \
void print() const { display(&ALOG); } \
void println() const { display(&NLOG); } \
void display(logfunc_t logger) const { logger(__VA_ARGS__); }