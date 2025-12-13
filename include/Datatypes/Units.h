#pragma once

// Usually a floating point datatype that is used to describe coordinates
typedef float Unit;

//
// New API
typedef float sre_unit;

#ifdef __cplusplus

namespace sre
{
	using unit = sre_unit;
}

constexpr sre::unit operator ""_ut(long double n) { return static_cast<sre::unit>(n); }

#endif