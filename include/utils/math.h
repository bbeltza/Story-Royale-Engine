#pragma once

#define ut_lerp(a, b, t) (a + (b - a) * t)

#define ut_bit(x) (1 << x)

#define ut_max(x, y) ( x < y ? y : x)
#define ut_min(x, y) ( x > y ? y : x)
#define ut_sign(x) (x < 0 ? -1 : x > 0 ? 1 : 0)

#define ut_clamp(x, min, max) (((x) > (max)) ? (max) : ((x) < (min)) ? (min) : (x))
#define ut_setclamp(x, min, max) x = ut_clamp(x, min, max)

// Math macro defines
#define UT_E        2.71828182845904523536   // e
#define UT_LOG2E    1.44269504088896340736   // log2(e)
#define UT_LOG10E   0.434294481903251827651  // log10(e)
#define UT_LN2      0.693147180559945309417  // ln(2)
#define UT_LN10     2.30258509299404568402   // ln(10)
#define UT_PI       3.14159265358979323846   // pi
#define UT_PI_2     1.57079632679489661923   // pi/2
#define UT_PI_4     0.785398163397448309616  // pi/4
#define UT_1_PI     0.318309886183790671538  // 1/pi
#define UT_2_PI     0.636619772367581343076  // 2/pi
#define UT_2_SQRTPI 1.12837916709551257390   // 2/sqrt(pi)
#define UT_SQRT2    1.41421356237309504880   // sqrt(2)
#define UT_SQRT1_2  0.707106781186547524401  // 1/sqrt(2)