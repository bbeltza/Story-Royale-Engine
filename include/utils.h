#pragma once

#ifndef lerp
#define lerp(a, b, t) (a + (b - a) * t)
#endif

#define bit(x) (1 << x)

#define max(x, y) ( x < y ? y : x)
#define min(x, y) ( x > y ? y : x)