#pragma once
#include <Datatypes/common.hpp>
#include <Datatypes/Units.h>

#include <math.h>

#include "utils/math.h"
#include "utils/math.hpp"

namespace sre
{
    template <typename T>
    using vec_valid_t = std::is_arithmetic<T>;

    template <typename T>
    struct vec2
    {
        static_assert(vec_valid_t<T>::value, "sre::vec2 datatype must be an arithmetic type: The type must represent a number");
        using type = T;

        T x = 0;
        T y = 0;

        constexpr vec2() = default;
        constexpr vec2(T x, T y): x(x), y(y) {}
        constexpr vec2(T scalar): x(scalar), y(scalar) {}
        constexpr vec2(const vec2& other): vec2(other.x, other.y) {}

        template <typename T2>
        constexpr vec2(T2 x, T2 y): vec2(static_cast<T>(x), static_cast<T>(y)) {}

        template <typename T2>
        explicit constexpr vec2(const vec2<T2>& other): vec2(other.x, other.y) {}

        template <typename T2>
        explicit constexpr operator vec2<T2>() const
        {
            return {
                static_cast<T2>(x),
                static_cast<T2>(y)
            };
        }

        template <typename T2> inline void add(const vec2<T2>& other) { x += static_cast<T>(other.x); y += static_cast<T>(other.y); }
        template <typename T2> inline void sub(const vec2<T2>& other) { x -= static_cast<T>(other.x); y -= static_cast<T>(other.y); }
        template <typename T2> inline void mul(const vec2<T2>& other) { x *= static_cast<T>(other.x); y *= static_cast<T>(other.y); }
        template <typename T2> inline void div(const vec2<T2>& other) { x /= static_cast<T>(other.x); y /= static_cast<T>(other.y); }
        template <typename T2> inline void add(T2 other) { x += static_cast<T>(other); y += static_cast<T>(other); }
        template <typename T2> inline void sub(T2 other) { x -= static_cast<T>(other); y -= static_cast<T>(other); }
        template <typename T2> inline void mul(T2 other) { x *= static_cast<T>(other); y *= static_cast<T>(other); }
        template <typename T2> inline void div(T2 other) { x /= static_cast<T>(other); y /= static_cast<T>(other); }
        inline void inverse() { x = -x; y = -y; }

        template <typename T2>
        constexpr auto getAdd(const vec2<T2>& other) const -> vec2<decltype(x + other.x)> { return { x + other.x, y + other.y }; }
        template <typename T2>
        constexpr auto getSub(const vec2<T2>& other) const -> vec2<decltype(x - other.x)> { return { x - other.x, y - other.y }; }
        template <typename T2>
        constexpr auto getMul(const vec2<T2>& other) const -> vec2<decltype(x * other.x)> { return { x * other.x, y * other.y }; }
        template <typename T2>
        constexpr auto getDiv(const vec2<T2>& other) const -> vec2<decltype(x / other.x)> { return { x / other.x, y / other.y }; }

        template <typename T2>
        constexpr auto getAdd(T2 other) const -> vec2<decltype(x + other)> { return { x + other, y + other }; }
        template <typename T2>
        constexpr auto getSub(T2 other) const -> vec2<decltype(x - other)> { return { x - other, y - other }; }
        template <typename T2>
        constexpr auto getMul(T2 other) const -> vec2<decltype(x * other)> { return { x * other, y * other }; }
        template <typename T2>
        constexpr auto getDiv(T2 other) const -> vec2<decltype(x / other)> { return { x / other, y / other }; }

        constexpr auto getInverse() const -> vec2 { return { -x, -y }; }

        constexpr T max() const { return ut_max(x, y); }
        constexpr T min() const { return ut_min(x, y); }
        constexpr vec2 clamp(const vec2& min, const vec2& max) const { return { ut_clamp(x, min.x, max.x), ut_clamp(y, min.y, max.y) }; }
        constexpr vec2 lerp(const vec2& dst, double alpha) const { return { ut_lerp(x, dst.x, alpha), ut_lerp(y, dst.y, alpha) }; }

        constexpr vec2 abs() const { return { ut::abs(x), ut::abs(y) }; }
        constexpr vec2 floor() const { return { std::floor(x), std::floor(y) }; }
        constexpr vec2 floor_grid(T size) const { return { std::floor(x/size) * size, std::floor(y/size) * size }; }
        template <typename T2>
        constexpr auto dot(const vec2<T2>& other) const -> decltype(x * other.x) { return x * other.x + y * other.y; }

        constexpr double magnitude() const { return hypot(x, y); }
        constexpr double getNormalized() const { return getDiv(magnitude()); }

        inline void normalize() { div(static_cast<T>(magnitude())); }
        inline void setclamp(const vec2& min, const vec2& max) { ut_setclamp(x, min.x, max.y); ut_setclamp(y, min.y, max.y); }
        inline void setlerp(const vec2& dst, double alpha) { x = static_cast<T> ut_lerp(x, dst.x, alpha); y = static_cast<T> ut_lerp(y, dst.y, alpha); }

        constexpr vec2<int> sign() const { return { ut_sign(x), ut_sign(y) }; }

        template <typename T2> constexpr auto operator +(const vec2<T2>& other) const -> decltype(getAdd(other)) { return getAdd(other); }
        template <typename T2> constexpr auto operator -(const vec2<T2>& other) const -> decltype(getSub(other)) { return getSub(other); }
        template <typename T2> constexpr auto operator *(const vec2<T2>& other) const -> decltype(getMul(other)) { return getMul(other); }
        template <typename T2> constexpr auto operator /(const vec2<T2>& other) const -> decltype(getDiv(other)) { return getDiv(other); }
        template <typename T2> constexpr auto operator +(T2 other) const -> decltype(getAdd(other)) { return getAdd(other); }
        template <typename T2> constexpr auto operator -(T2 other) const -> decltype(getAdd(other)) { return getSub(other); }
        template <typename T2> constexpr auto operator *(T2 other) const -> decltype(getAdd(other)) { return getMul(other); }
        template <typename T2> constexpr auto operator /(T2 other) const -> decltype(getAdd(other)) { return getDiv(other); }

        template <typename T2> inline void operator +=(const vec2<T2>& other) { add(other); }
        template <typename T2> inline void operator -=(const vec2<T2>& other) { sub(other); }
        template <typename T2> inline void operator *=(const vec2<T2>& other) { mul(other); }
        template <typename T2> inline void operator /=(const vec2<T2>& other) { div(other); }
        template <typename T2> inline void operator +=(T2 other) { add(other); }
        template <typename T2> inline void operator -=(T2 other) { sub(other); }
        template <typename T2> inline void operator *=(T2 other) { mul(other); }
        template <typename T2> inline void operator /=(T2 other) { div(other); }

        inline vec2 operator -() const { return getInverse(); }

        constexpr bool operator ==(const vec2& other) const { return x == other.x && y == other.y; }
        constexpr bool operator !=(const vec2& other) const { return x != other.x || y != other.y; }

        constexpr operator const T*() const { return &x; }
        inline operator T*() { return &x; }

        static const vec2 ZERO;
        static const vec2 ONE;
        static const vec2 CENTER;
    };

    template <class T> const vec2<T> vec2<T>::ZERO = { 0, 0 };
    template <class T> const vec2<T> vec2<T>::ONE = { 1, 1 };
    template <class T> const vec2<T> vec2<T>::CENTER = { 0.5, 0.5 };

    using vec2i = vec2<int>;
    using vec2u = vec2<unsigned>;
    using vec2f = vec2<float>;
    using vec2d = vec2<double>;

    using vec2ut = vec2<unit>;
}

#include <ostream>

template <typename Char, typename Traits, typename T>
std::basic_ostream<Char, Traits>& operator <<(std::basic_ostream<Char, Traits>& os, const sre::vec2<T>& pos)
{
    os << "{ " << (pos.x + 0) << " ; " << (pos.y + 0) << " }";
    return os;
}