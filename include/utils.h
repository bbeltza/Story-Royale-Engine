#pragma once

#ifndef lerp
#define lerp(a, b, t) (a + (b - a) * t)
#endif

#define bit(x) (1 << x)

#ifndef mmax
#define mmax(x, y) ( x < y ? y : x)
#endif
#ifndef mmin
#define mmin(x, y) ( x > y ? y : x)
#endif