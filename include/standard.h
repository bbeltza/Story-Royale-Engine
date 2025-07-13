#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <cstring>

#include <vector>
#include <list>
#include <unordered_map>
#include <array>

#define sign(x) ((0 < x) - (x < 0))
#define RES_PREFIX "res://"

#ifdef __GNUC__
#define strcpy_s(dest, n, src) strncpy(dest, src, n)
#define strcat_s(dest, n, src) strncat(dest, src, n)
#endif