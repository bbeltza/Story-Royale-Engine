#pragma once
#include <math.h>
#include <type_traits>

#include "utils/logging.h"
#include "utils/math.h"

template <class _Num>
struct Vector2
{
    static_assert(std::is_arithmetic<_Num>::value, "Vector2 datatype must be an arithmetic type: The type must represent a number");

    constexpr Vector2(const _Num scalar): X(scalar), Y(scalar) {}
    constexpr Vector2(const _Num x, const _Num y): X(x), Y(y) {}
    constexpr Vector2(const Vector2& other): Vector2<_Num>(other.X, other.Y) {}
    constexpr Vector2(): Vector2(0, 0) {}
    
    template <class _Num2> constexpr Vector2(const _Num2 x, const _Num2 y): X((_Num)x), Y((_Num)y) {}
    template <class _Num2> constexpr Vector2(const Vector2<_Num2> other): Vector2(other.X, other.Y) {}

    template <class _Num2> Vector2& operator =(const Vector2<_Num2>& other)
    {
        X = static_cast<_Num>(other.X);
        Y = static_cast<_Num>(other.Y);
        return *this;
    }

    _Num X, Y;

    inline void Add(const Vector2& other) { X += other.X; Y += other.Y; }
    inline void Sub(const Vector2& other) { X -= other.X; Y -= other.Y; }
    inline void Mul(const Vector2& other) { X *= other.X; Y *= other.Y; }
    inline void Div(const Vector2& other) { X /= other.X; Y /= other.Y; }
    inline void Add(const _Num other) { X += other; Y += other; }
    inline void Sub(const _Num other) { X -= other; Y -= other; }
    inline void Mul(const _Num other) { X *= other; Y *= other; }
    inline void Div(const _Num other) { X /= other; Y /= other; }

    inline Vector2 getAdd(const Vector2& other) const { return {X + other.X, Y + other.Y}; }
    inline Vector2 getSub(const Vector2& other) const { return {X - other.X, Y - other.Y}; }
    inline Vector2 getMul(const Vector2& other) const { return {X * other.X, Y * other.Y}; }
    inline Vector2 getDiv(const Vector2& other) const { return {X / other.X, Y / other.Y}; }
    inline Vector2 getAdd(const _Num other) const { return {X + other, Y + other}; }
    inline Vector2 getSub(const _Num other) const { return {X - other, Y - other}; }
    inline Vector2 getMul(const _Num other) const { return {X * other, Y * other}; }
    inline Vector2 getDiv(const _Num other) const { return {X / other, Y / other}; }

    inline void Print() const {display(&ALOG);}
    inline void PrintLn() const {display(&NLOG);}

    inline double getMagnitude() {return sqrt(X*X + Y*Y);}

    inline void LerpTo(const Vector2& other, float alpha, const Vector2& origin) { X = ut_lerp(origin.X, other.X, alpha); Y = ut_lerp(origin.Y, other.Y, alpha); }
    inline void LerpTo(const Vector2& other, float alpha) { LerpTo(other, alpha, *this); }
    inline Vector2 Lerp(const Vector2& other, float alpha) const {return {ut_lerp(X, other.X, alpha), ut_lerp(Y, other.Y, alpha)};}

    inline _Num getMin() const {return X < Y ? X : Y;}
    inline _Num getMax() const {return X > Y ? X : Y;}

    inline bool isZero() const {return !(X || Y);}

    // A vector with its components set to 0
    static const Vector2<_Num> ZERO;
    // A vector with its components set to 1
    static const Vector2<_Num> ONE;
    // A vector with its components set to 0.5, however, if the vector should be composed of floating point types, otherwise the components will be rounded to 0
    static const Vector2<_Num> CENTER;

    private:
    void display(logfunc_t _printer) const {_printer("{ %g, %g }", (double)X, (double)Y);}
};

#define _t template <class _Num>
#define _t2 template <class _Num, class _NumX>
#define vec2 Vector2<_Num>
#define vec2x Vector2<_NumX>

// Static

_t const vec2 vec2::ZERO = {0};
_t const vec2 vec2::ONE = {1};
_t const vec2 vec2::CENTER = {0.5};

// Built-in operators

#define vec2op(op) -> Vector2<decltype(first.X op other.X)> { return {first.X op other.X, first.Y op other.Y}; }
#define vec2op_num(op) -> Vector2<decltype(first.X op other)> { return {first.X op other, first.Y op other}; }
#define vec2aop(op) { first.X op other.X; first.Y op other.Y; return first; }
#define vec2aop_num(op) { first.X op other; first.Y op other; return first; }

_t2 inline auto operator +(const vec2& first, const vec2x& other) vec2op(+)
_t2 inline auto operator -(const vec2& first, const vec2x& other) vec2op(-)
_t2 inline auto operator *(const vec2& first, const vec2x& other) vec2op(*)
_t2 inline auto operator /(const vec2& first, const vec2x& other) vec2op(/)

_t2 inline auto operator +(const vec2& first, const _NumX other) vec2op_num(+)
_t2 inline auto operator -(const vec2& first, const _NumX other) vec2op_num(-)
_t2 inline auto operator *(const vec2& first, const _NumX other) vec2op_num(*)
_t2 inline auto operator /(const vec2& first, const _NumX other) vec2op_num(/)

_t2 inline vec2& operator +=(vec2& first, const vec2x& other) vec2aop(+=)
_t2 inline vec2& operator -=(vec2& first, const vec2x& other) vec2aop(-=)
_t2 inline vec2& operator *=(vec2& first, const vec2x& other) vec2aop(*=)
_t2 inline vec2& operator /=(vec2& first, const vec2x& other) vec2aop(/=)

_t2 inline vec2& operator +=(vec2& first, const _NumX other) vec2aop_num(+=)
_t2 inline vec2& operator -=(vec2& first, const _NumX other) vec2aop_num(-=)
_t2 inline vec2& operator *=(vec2& first, const _NumX other) vec2aop_num(*=)
_t2 inline vec2& operator /=(vec2& first, const _NumX other) vec2aop_num(/=)

#undef vec2op
#undef vec2op_num
#undef vec2
#undef _t

// Template type definitions

typedef Vector2<int> Vector2i;
typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;

typedef Vector2<unsigned int> Vector2u;

#include "Datatypes/Units.h"

typedef Vector2<Unit> Vector2ut;
