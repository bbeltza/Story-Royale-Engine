#pragma once
#include "datatypes/vector.hpp"

namespace sre
{
    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type>
    constexpr T abs(T x) { return x; }
    template <typename T> constexpr T abs(T x) { return x < 0 ? -x : x; }
    template <typename T> constexpr double rad(T x) { return (x * (M_PI / 180)); }

    template <typename T>
    struct rect2D
    {
        SRE_IMPLEMENT_DATATYPE("{ {%g, %g} ; {%g, %g} }",
            static_cast<double>(position.x),
            static_cast<double>(position.y),
            static_cast<double>(size.x),
            static_cast<double>(size.y))

        using type = T;
        using vec = vec2<type>;

        vec position;
        vec size;

        constexpr rect2D() = default;
        constexpr rect2D(T x, T y, T w, T h): position(x, y), size(w, h) {}
        constexpr rect2D(T xy, T wh): position(xy), size(wh) {}
        constexpr rect2D(const vec& position, const vec& size): position(position), size(size) {}
        constexpr rect2D(const rect2D& copy): position(copy.position), size(copy.size) {}

        template <typename T2>
        constexpr operator rect2D<T2>() const
        {
            return {
                static_cast<rect2D<T2>::vec>(position),
                static_cast<rect2D<T2>::vec>(size)
            };
        }

        constexpr bool intersects(const rect2D& other) const {
            return abs(position.x - other.position.x) < size.x / 2 + other.size.x / 2 &&
                abs(position.y - other.position.y) < size.y / 2 + other.size.y / 2;
        }

        constexpr T top() const { return position.y - abs(size.y) / 2; }
        constexpr T bottom() const { return position.y + abs(size.y) / 2; }
        constexpr T left() const { return position.x - abs(size.x) / 2; }
        constexpr T right() const { return position.x + abs(size.x) / 2; }

        constexpr vec top_left() const { return {left(), top()}; }
        constexpr vec top_right() const { return {right(), top()}; }
        constexpr vec bottom_left() const { return {left(), bottom()}; }
        constexpr vec bottom_right() const { return {right(), bottom()}; }

        constexpr vec xrotated_offset(double angle) const { return vec2d{ abs(size.x) / 2 * cos(rad(angle)), abs(size.x) / 2 * sin(rad(angle)) }; }
        constexpr vec yrotated_offset(double angle) const { return vec2d{ abs(size.y) / 2 * cos(rad(angle + 90)), abs(size.y) / 2 * sin(rad(angle + 90)) }; }

        constexpr vec rotated_left(double angle) const { return position - xrotated_offset(angle); }
        constexpr vec rotated_right(double angle) const { return position + xrotated_offset(angle); }
        constexpr vec rotated_top(double angle) const { return position - yrotated_offset(angle); }
        constexpr vec rotated_bottom(double angle) const { return position + yrotated_offset(angle); }

        constexpr vec rotated_topleft(double angle) const { return position - xrotated_offset(angle) - yrotated_offset(angle); }
        constexpr vec rotated_topright(double angle) const { return position + xrotated_offset(angle) - yrotated_offset(angle); }
        constexpr vec rotated_bottomleft(double angle) const { return position - xrotated_offset(angle) + yrotated_offset(angle); }
        constexpr vec rotated_bottomright(double angle) const { return position + xrotated_offset(angle) + yrotated_offset(angle); }
    };

    // A rect2D datatype composed of ints.
    using rect2Di = rect2D<int>;
    // A rect2D datatype composed of floats.
    using rect2Df = rect2D<float>;
    using rect2Dd = rect2D<double>;

    // A rect2D datatype composed of units ( floats for now )
    using rect2Dut = rect2D<unit>;
}

/*
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

*/