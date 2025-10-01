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

/* 
Creates a null terminated array, of anything, takes any argument to put into an array in which will add NULL at the end, to be able to iterate without having to speficy a size.

It's discouraged to use it with numbers as numbers can be NULL too
*/
#define n_array(...) {__VA_ARGS__, NULL}