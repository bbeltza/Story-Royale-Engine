#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <cstring>

#include <vector>
#include <list>
#include <array>

#include <set>
#include <unordered_map>

extern "C"
{
    #include "C/extra_string.h"
    #include "C/delta_model.h"
}

#ifndef sign
#define sign(x) ((0 < x) - (x < 0))
#endif
#ifndef lerp
#define lerp(a, b, t) (a + (b - a) * t)
#endif

#define RES_PREFIX "res://"

#ifndef _MSC_VER
#define strcpy_s(dest, n, src) strncpy(dest, src, n)
#define strcat_s(dest, n, src) strncat(dest, src, n)
#endif