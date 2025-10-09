#pragma once

#define ut_lerp(a, b, t) (a + (b - a) * t)

#define ut_bit(x) (1 << x)

#define ut_max(x, y) ( x < y ? y : x)
#define ut_min(x, y) ( x > y ? y : x)
#define ut_sign(x) (x < 0 ? -1 : x > 0 ? 1 : 0)

/* 
Creates a null terminated array, of anything, takes any argument to put into an array in which will add NULL at the end, to be able to iterate without having to speficy a size.

It's discouraged to use it with numbers as numbers can be NULL too
*/
#define ut_array(...) {__VA_ARGS__, NULL}

#define ut_clamp(x, min, max) (((x) > (max)) ? (max) : ((x) < (min)) ? (min) : (x))
#define ut_setclamp(x, min, max) x = ut_clamp(x, min, max)