#pragma once
#include "Datatypes/Vector.hpp"
#include "utils/math.hpp"

namespace sre
{
    template <typename T>
    struct rect2D
    {
        using type = T;
        using vec = vec2<type>;

        vec position;
        vec size { 100, 100 };

        constexpr rect2D() = default;
        constexpr rect2D(T x, T y, T w, T h): position(x, y), size(w, h) {}
        constexpr rect2D(T xy, T wh): position(xy), size(wh) {}
        constexpr rect2D(const vec& position, const vec& size): position(position), size(size) {}
        constexpr rect2D(const rect2D& copy): position(copy.position), size(copy.size) {}

        template <typename T2>
        explicit constexpr rect2D(const rect2D<T2>& other): position(other.position), size(other.size) {}

        template <typename T2>
        explicit constexpr operator rect2D<T2>() const
        {
            return {
                static_cast<typename rect2D<T2>::vec>(position),
                static_cast<typename rect2D<T2>::vec>(size)
            };
        }

        constexpr bool intersects(const rect2D& other) const {
            #if __cplusplus < 201300
                #define abs_size (size.abs() / 2)
                #define abs_othersize (other.size.abs() / 2)
            #else
                const vec abs_size = size.abs() / 2;
                const vec abs_othersize = other.size.abs() / 2;
            #endif
            return ut::abs(position.x - other.position.x) < abs_size.x + abs_othersize.x &&
                ut::abs(position.y - other.position.y) < abs_size.y + abs_othersize.y;
        }

        constexpr bool intersects(const vec pt) const {
            #ifndef abs_size
                const vec abs_size = size.abs() / 2;
            #endif
            return (pt.x >= position.x - abs_size.x && pt.x <= position.x + abs_size.x) &&
                    (pt.y >= position.y - abs_size.y && pt.y <= position.y + abs_size.y);
        }
        
        constexpr bool simple_intersects(const vec pt) const {
            #ifdef abs_size
                #undef abs_size
                #define abs_size (size.abs())
            #else
                const vec abs_size = size.abs();
            #endif
            return ( pt.x >= position.x && pt.x <= position.x + abs_size.x ) &&
                    (pt.y >= position.y && pt.y <= position.y + abs_size.y);
        }

        #ifdef abs_size
            #undef abs_size
            #undef abs_othersize
        #endif

        constexpr T top() const { return position.y - ut::abs(size.y) / 2; }
        constexpr T bottom() const { return position.y + ut::abs(size.y) / 2; }
        constexpr T left() const { return position.x - ut::abs(size.x) / 2; }
        constexpr T right() const { return position.x + ut::abs(size.x) / 2; }

        constexpr vec top_left() const { return {left(), top()}; }
        constexpr vec top_right() const { return {right(), top()}; }
        constexpr vec bottom_left() const { return {left(), bottom()}; }
        constexpr vec bottom_right() const { return {right(), bottom()}; }

        constexpr vec xrotated_offset(double angle) const { return vec{ ut::abs(size.x) / 2 * cos(ut::rad(angle)), ut::abs(size.x) / 2 * sin(ut::rad(angle)) }; }
        constexpr vec yrotated_offset(double angle) const { return vec{ ut::abs(size.y) / 2 * cos(ut::rad(angle + 90)), ut::abs(size.y) / 2 * sin(ut::rad(angle + 90)) }; }

        constexpr vec rotated_left(double angle) const { return position - xrotated_offset(angle); }
        constexpr vec rotated_right(double angle) const { return position + xrotated_offset(angle); }
        constexpr vec rotated_top(double angle) const { return position - yrotated_offset(angle); }
        constexpr vec rotated_bottom(double angle) const { return position + yrotated_offset(angle); }

        constexpr vec rotated_topleft(double angle) const { return position - xrotated_offset(angle) - yrotated_offset(angle); }
        constexpr vec rotated_topright(double angle) const { return position + xrotated_offset(angle) - yrotated_offset(angle); }
        constexpr vec rotated_bottomleft(double angle) const { return position - xrotated_offset(angle) + yrotated_offset(angle); }
        constexpr vec rotated_bottomright(double angle) const { return position + xrotated_offset(angle) + yrotated_offset(angle); }

        constexpr bool operator ==(const rect2D& other) const { return position == other.position && size == other.size; }
        constexpr bool operator !=(const rect2D& other) const { return position != other.position || size != other.size; }
    };

    // A rect2D datatype composed of ints.
    using rect2Di = rect2D<int>;
    using rect2Du = rect2D<unsigned>;
    // A rect2D datatype composed of floats.
    using rect2Df = rect2D<float>;
    using rect2Dd = rect2D<double>;

    // A rect2D datatype composed of units ( floats for now )
    using rect2Dut = rect2D<unit>;

    // NOTE: This returns a centered rect!!
    template <typename T>
    constexpr rect2D<T> from_bounds(const vec2<T>& top_left, const vec2<T>& bottom_right)
    {
        return {
            top_left + (bottom_right - top_left)/2,
            (bottom_right - top_left)
        };
    }
}

template <typename Char, typename Traits, typename T>
std::basic_ostream<Char, Traits>& operator <<(std::basic_ostream<Char, Traits>& os, const sre::rect2D<T>& rect)
{
    os << "( " << (rect.position.x + 0) << " ; " << (rect.position.y + 0) << " | " << (rect.size.x + 0) << " ; " << (rect.size.y) << " )";
    return os;
}