#pragma once
#include <Base/Log.h>
#include <type_traits>
#include <ints.h>

#define SRE_IMPLEMENT_DATATYPE(...) \
inline void display() const { sre::log<sre::LOGCATEGORY_DEBUG>(__VA_ARGS__); }
