#pragma once
#include <math.h>
#include <iostream>

template <class _Num>
struct Vector2
{
    Vector2(const _Num x, const _Num y): X(x), Y(y) {}
    Vector2(const Vector2& other): Vector2<_Num>(other.X, other.Y) {}
    Vector2(): Vector2(0, 0) {}
    
    template <class _Num2> Vector2(const _Num2 x, const _Num2 y): X(x), Y(y) {}
    template <class _Num2> Vector2(const Vector2<_Num2> other): Vector2(other.X, other.Y) {}

    inline void Print() const {std::cout << "{ " << X << ", " << Y << " }\n";}

    inline double getMagnitude() {return sqrt(X*X + Y*Y);}

    _Num X, Y;
};

#define vec2op(op) { return {first.X op other.X, first.Y op other.Y}; }
#define vec2op_num(op) { return {first.X op other, first.Y op other}; }
#define vec2 Vector2<_Num>
#define _t template <class _Num>

_t inline vec2 operator +(const vec2& first, const vec2& other) vec2op(+)
_t inline vec2 operator -(const vec2& first, const vec2& other) vec2op(-)
_t inline vec2 operator *(const vec2& first, const vec2& other) vec2op(*)
_t inline vec2 operator /(const vec2& first, const vec2& other) vec2op(/)

_t inline vec2 operator +(const vec2& first, const _Num& other) vec2op_num(+)
_t inline vec2 operator -(const vec2& first, const _Num& other) vec2op_num(-)
_t inline vec2 operator *(const vec2& first, const _Num& other) vec2op_num(*)
_t inline vec2 operator /(const vec2& first, const _Num& other) vec2op_num(/)

#undef vec2op
#undef vec2op_num
#undef vec2
#undef _t

typedef Vector2<int> Vector2i;
typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;