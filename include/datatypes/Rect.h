#pragma once
#include "Vector.h"

#define r_pi M_PI

template <typename _Num> inline _Num _abs(_Num x) {return x < 0 ? -x : x;}
template <typename _Num> inline double _rad(_Num x) {return (x * (r_pi / 180));}

#define vec2 Vector2<_Num>

template <typename _Num>
struct Rect
{
    Rect(): Position(0, 0), Size(50, 50) {}
    Rect(const _Num x, const _Num y, const _Num width, const _Num height): Position(x, y), Size(width, height) {}
    Rect(const _Num xy, const _Num wh): Position(xy, xy), Size(wh, wh) {}
    Rect(const Vector2<_Num> &position, const Vector2<_Num> &size): Position(position), Size(size) {}
    Rect(const Rect& other): Position(other.Position), Size(other.Size) {}
    template <class _OtherNum> Rect(const Rect<_OtherNum>& other): Position(other.Position), Size(other.Size) {}

    vec2 Position, Size;
    
    inline bool Intersects(const Rect& other) const {
        return _abs(this->Position.X - other.Position.X) < this->Size.X/2 + other.Size.X/2 &&
        _abs(this->Position.Y - other.Position.Y) < this->Size.Y/2 + other.Size.Y/2;
    }

    inline constexpr _Num getTop() const {return this->Position.Y - this->Size.Y/2;}
    inline constexpr _Num getBottom() const {return this->Position.Y + this->Size.Y/2;}
    inline constexpr _Num getLeft() const {return this->Position.X - this->Size.X/2;}
    inline constexpr _Num getRight() const {return this->Position.X + this->Size.X/2;}

    inline constexpr vec2 getTopLeft() const {return {this->getLeft(), this->getTop()};}
    inline constexpr vec2 getTopRight() const {return {this->getRight(), this->getTop()};}
    inline constexpr vec2 getBottomLeft() const {return {this->getLeft(), this->getBottom()};}
    inline constexpr vec2 getBottomRight() const {return {this->getRight(), this->getBottom()};}

    inline constexpr vec2 getXRotatedOffset(double angle) const {return {this->Size.X/2 * cos(_rad(angle)), this->Size.X/2 * sin(_rad(angle))};}
    inline constexpr vec2 getYRotatedOffset(double angle) const {return {this->Size.Y/2 * cos(_rad(angle + 90)), this->Size.Y/2 * sin(_rad(angle + 90))};}

    inline constexpr vec2 getLeftRotated(double angle) const {return this->Position - getXRotatedOffset(angle);}
    inline constexpr vec2 getRightRotated(double angle) const {return this->Position + getXRotatedOffset(angle);}
    inline constexpr vec2 getTopRotated(double angle) const {return this->Position - getYRotatedOffset(angle);}
    inline constexpr vec2 getBottomRotated(double angle) const {return this->Position + getYRotatedOffset(angle);}

    inline constexpr vec2 getTopLeftRotated(double angle) const {return this->Position - getXRotatedOffset(angle) - getYRotatedOffset(angle);}
    inline constexpr vec2 getTopRightRotated(double angle) const {return this->Position + getXRotatedOffset(angle) - getYRotatedOffset(angle);}
    inline constexpr vec2 getBottomLeftRotated(double angle) const {return this->Position - getXRotatedOffset(angle) + getYRotatedOffset(angle);}
    inline constexpr vec2 getBottomRightRotated(double angle) const {return this->Position + getXRotatedOffset(angle) + getYRotatedOffset(angle);}

};

#undef vec2

// A Rect datatype composed of 32-bit integers.
typedef Rect<int> RectI;
// A Rect datatype composed of floats.
typedef Rect<float> RectF;