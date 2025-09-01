#pragma once
#include <math.h>
#include <iostream>

template <class _Num>
struct Vector2
{
    Vector2(const _Num scalar): X(scalar), Y(scalar) {}
    Vector2(const _Num x, const _Num y): X(x), Y(y) {}
    Vector2(const Vector2& other): Vector2<_Num>(other.X, other.Y) {}
    Vector2(): Vector2(0, 0) {}
    
    template <class _Num2> Vector2(const _Num2 x, const _Num2 y): X((_Num)x), Y((_Num)y) {}
    template <class _Num2> Vector2(const Vector2<_Num2> other): Vector2(other.X, other.Y) {}

    inline void Print() const {printf("{ %g, %g }\n", (double)X, (double)Y);}

    inline double getMagnitude() {return sqrt(X*X + Y*Y);}

    inline Vector2 Lerp(const Vector2& other, float alpha) {return *this + (other - *this) * alpha;}

    inline _Num getMin() const {return X < Y ? X : Y;}
    inline _Num getMax() const {return X > Y ? X : Y;}

    inline operator bool() const {return X && Y;}

    static const Vector2<_Num> ZERO;
    static const Vector2<_Num> ONE;

    _Num X, Y;
};

#define vec2 Vector2<_Num>
#define _t template <class _Num>
#define _t2 template <class _Num, class _Num2>

// Static

_t const vec2 vec2::ZERO = {0};
_t const vec2 vec2::ONE = {1};

// Built-in operators

#define vec2op(op) { return {first.X op other.X, first.Y op other.Y}; }
#define vec2op_num(op) { return {first.X op other, first.Y op other}; }
#define vec2aop(op) { first.X op= other.X; first.Y op= other.Y; return first; }

_t inline vec2 operator +(const vec2& first, const vec2& other) vec2op(+)
_t inline vec2 operator -(const vec2& first, const vec2& other) vec2op(-)
_t inline vec2 operator *(const vec2& first, const vec2& other) vec2op(*)
_t inline vec2 operator /(const vec2& first, const vec2& other) vec2op(/)

_t2 inline vec2 operator +(const vec2& first, const _Num2 other) vec2op_num(+)
_t2 inline vec2 operator -(const vec2& first, const _Num2 other) vec2op_num(-)
_t2 inline vec2 operator *(const vec2& first, const _Num2 other) vec2op_num(*)
_t2 inline vec2 operator /(const vec2& first, const _Num2 other) vec2op_num(/)

_t inline vec2& operator +=(vec2& first, const vec2& other) { first.X += other.X; first.Y += other.Y; return first; }
_t inline vec2& operator -=(vec2& first, const vec2& other) { first.X -= other.X; first.Y -= other.Y; return first; }
_t inline vec2& operator *=(vec2& first, const vec2& other) { first.X *= other.X; first.Y *= other.Y; return first; }
_t inline vec2& operator /=(vec2& first, const vec2& other) { first.X /= other.X; first.Y /= other.Y; return first; }

#undef vec2op
#undef vec2op_num
#undef vec2
#undef _t

// Template type definitions

typedef Vector2<int> Vector2i;
typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;

typedef Vector2<unsigned int> Vector2u;