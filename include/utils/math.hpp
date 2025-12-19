#pragma once
#include <type_traits>

#include "utils/math.h"

namespace ut
{
    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type>
    constexpr T abs(T x) { return x; }
    template <typename T> constexpr T abs(T x) { return x < 0 ? -x : x; }
    template <typename T> constexpr double rad(T x) { return (x * (UT_PI / 180)); }
}