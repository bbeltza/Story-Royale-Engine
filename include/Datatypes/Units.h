#pragma once

// Usually a floating point datatype that is used to describe coordinates
typedef float sre_unit;

#ifdef __cplusplus

namespace sre
{
	using unit = sre_unit;
}

constexpr sre::unit operator ""_ut(long double n) { return static_cast<sre::unit>(n); }

#define SRE_UT(x) x##_ut

#else
	#define SRE_UT(x) (sre_unit)(x)
#endif