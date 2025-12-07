#pragma once
#include "Vector.hpp"

template <typename _Num> inline _Num _abs(_Num x) {return x < 0 ? -x : x;}
template <typename _Num> inline double _rad(_Num x) {return (x * (M_PI / 180));}

#define vec2 Vector2<_Num>

template <typename _Num>
struct Rect
{
    using type = _Num;

    void Print() const { display(&ALOG); }
    void PrintLn() const { display(&NLOG); }

    constexpr Rect(): Position(0, 0), Size(50, 50) {}
    constexpr Rect(_Num x, _Num y, _Num width, _Num height): Position(x, y), Size(width, height) {}
    constexpr Rect(_Num xy, _Num wh): Position(xy, xy), Size(wh, wh) {}
    constexpr Rect(const Vector2<_Num> &position, const Vector2<_Num> &size): Position(position), Size(size) {}
    constexpr Rect(const Rect& other): Position(other.Position), Size(other.Size) {}

    template <class _Num2>
    constexpr Rect(const Rect<_Num2>& other): Position(other.Position), Size(other.Size) {}
    template <class _Num2>
    constexpr Rect(_Num2 x, _Num2 y, _Num2 width, _Num2 height): Rect(
        static_cast<_Num>(x),
        static_cast<_Num>(y),
        static_cast<_Num>(width),
        static_cast<_Num>(height)
    ) {}

    vec2 Position, Size;
    
    constexpr bool Intersects(const Rect& other) const {
        return _abs(this->Position.X - other.Position.X) < this->Size.X/2 + other.Size.X/2 &&
        _abs(this->Position.Y - other.Position.Y) < this->Size.Y/2 + other.Size.Y/2;
    }

    Rect& reCenter() { Position = getBottomRight(); return *this; }

    constexpr Rect getCentered() const { return {getBottomRight(), Size}; }
    constexpr _Num absSizeX() const { return abs(this->Size.X); }
    constexpr _Num absSizeY() const { return abs(this->Size.Y); }

    constexpr _Num getTop() const {return this->Position.Y - absSizeY()/2;}
    constexpr _Num getBottom() const {return this->Position.Y + absSizeY() /2;}
    constexpr _Num getLeft() const {return this->Position.X - absSizeX()/2;}
    constexpr _Num getRight() const {return this->Position.X + absSizeX()/2;}

    constexpr vec2 getTopLeft() const {return {this->getLeft(), this->getTop()};}
    constexpr vec2 getTopRight() const {return {this->getRight(), this->getTop()};}
    constexpr vec2 getBottomLeft() const {return {this->getLeft(), this->getBottom()};}
    constexpr vec2 getBottomRight() const {return {this->getRight(), this->getBottom()};}

    constexpr vec2 getXRotatedOffset(double angle) const {return { absSizeX()/2 * cos(_rad(angle)), absSizeX()/2 * sin(_rad(angle))};}
    constexpr vec2 getYRotatedOffset(double angle) const {return { absSizeY()/2 * cos(_rad(angle + 90)), absSizeY()/2 * sin(_rad(angle + 90))};}

    constexpr vec2 getLeftRotated(double angle) const {return this->Position - getXRotatedOffset(angle);}
    constexpr vec2 getRightRotated(double angle) const {return this->Position + getXRotatedOffset(angle);}
    constexpr vec2 getTopRotated(double angle) const {return this->Position - getYRotatedOffset(angle);}
    constexpr vec2 getBottomRotated(double angle) const {return this->Position + getYRotatedOffset(angle);}

    constexpr vec2 getTopLeftRotated(double angle) const {return this->Position - getXRotatedOffset(angle) - getYRotatedOffset(angle);}
    constexpr vec2 getTopRightRotated(double angle) const {return this->Position + getXRotatedOffset(angle) - getYRotatedOffset(angle);}
    constexpr vec2 getBottomLeftRotated(double angle) const {return this->Position - getXRotatedOffset(angle) + getYRotatedOffset(angle);}
    constexpr vec2 getBottomRightRotated(double angle) const {return this->Position + getXRotatedOffset(angle) + getYRotatedOffset(angle);}

    public:

    static const Rect<_Num> ZERO;

    private:
    void display(logfunc_t _printer) const { _printer("{ {%g, %g} ; {%g, %g}", (double)Position.X, (double)Position.Y, (double)Size.X, (double)Size.Y); }
};

template <typename _Num>
const Rect<_Num> Rect<_Num>::ZERO = {0, 0, 0, 0};

#undef vec2

// A Rect datatype composed of 32-bit integers.
typedef Rect<int> RectI;
// A Rect datatype composed of floats.
typedef Rect<float> RectF;

typedef Rect<Unit> RectUt;