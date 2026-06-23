#ifndef SRE_UT_MATH_HPP
#define SRE_UT_MATH_HPP

#include <utils/math.h>
#include <type_traits>

namespace ut
{
    template <typename T> using abs_stype = typename std::enable_if<std::is_signed<T>::value, T>::type;
    template <typename T> using abs_utype = typename std::enable_if<std::is_unsigned<T>::value, T>::type;
    template <typename T> using arithmetic_type = typename std::enable_if<std::is_arithmetic<T>::value, T>::type;

    template <typename T>
    constexpr abs_utype<T> abs(T x) { return x; }
    template <typename T>
    constexpr abs_stype<T> abs(T x) { return x < 0 ? -x : x; }
    template <typename T> constexpr double rad(T x) { return ut_rad(x); }

    template <typename T>
    constexpr auto lerp(T src, T goal, float alpha) -> decltype(src.lerp(goal, alpha)) { return src.lerp(goal, alpha); }
    template <typename T>
    constexpr arithmetic_type<T> lerp(T src, T goal, float alpha) { return static_cast<T>(ut_lerp(src, goal, alpha)); }

    template <typename Tx, typename Ty>
    constexpr auto max(Tx&& a, Ty&& b) -> decltype(ut_max(a, b)) { return ut_max(a, b); }
    template <typename Tx, typename Ty>
    constexpr auto min(Tx&& a, Ty&& b) -> decltype(ut_min(a, b)) { return ut_min(a, b); }
}

#endif