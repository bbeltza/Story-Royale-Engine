#pragma once

template <typename _Num> inline _Num _abs(_Num x) {return x < 0 ? -x : x;}

template <typename _Num>
struct Rect
{
    Rect(): Position{0, 0}, Size{50, 50} {}
    Rect(const _Num x, const _Num y, const _Num width, const _Num height): Position{x, y}, Size{width, height} {}
    Rect(const _Num xy, const _Num wh): Position{xy, xy}, Size{wh, wh} {}
    Rect(const Rect& other): Position{other.Position}, Size{other.Size} {}
    template <class _OtherNum> Rect(const Rect<_OtherNum>& other): Position{other.Position}, Size{other.Size} {}

    inline bool Intersects(const Rect& other) const {
        printf("%d, ", _abs(this->Position.X - other.Position.X));
        printf("%d\n", _abs(this->Position.Y - other.Position.Y));

        return _abs(this->Position.X - other.Position.X) < this->Size.Width/2 + other.Size.Width/2 &&
        _abs(this->Position.Y - other.Position.Y) < this->Size.Height/2 + other.Size.Height/2;
    }

    struct
    {
        _Num X, Y;
    } Position;
    struct
    {
        _Num Width, Height;
    } Size;
    
};

// A Rect datatype composed of 32-bit integers.
typedef Rect<int> RectI;
// A Rect datatype composed of floats.
typedef Rect<float> RectF;