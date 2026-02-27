#pragma once
#include <Base/Log.h>
#include <ints.h>

#define SRE_IMPLEMENT_DATATYPE(...) \
inline void display() const { sre::log<sre::LOGCATEGORY_DEBUG>(__VA_ARGS__); }
