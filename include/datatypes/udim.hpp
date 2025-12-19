#pragma once
#include <datatypes/vector.hpp>
#include <datatypes/units.h>

namespace sre
{
    struct udim
    {
        using unit_type = ::sre::unit;

        constexpr udim() = default;
        constexpr udim(unit_type scale, unit_type offset): scale(scale), offset(offset) {}
        constexpr udim(const udim& copy): scale(copy.scale), offset(copy.offset) {}

        template <typename TS, typename TO>
        constexpr udim(TO scale, TS offset): udim( static_cast<unit_type>(scale), static_cast<unit_type>(offset) ) {}

        unit_type scale = 0;
        unit_type offset = 0;

        constexpr ::sre::unit to_absolute(unit_type relative) const { return relative * scale + offset; }

        static const udim ZERO;
        static const udim FULL;
    };
    struct udim2
    {
        using unit_type = udim::unit_type;

        constexpr udim2() = default;
        constexpr udim2(const udim& x, const udim& y): x(x), y(y) {}
        constexpr udim2(unit_type xscale, unit_type xoffset, unit_type yscale, unit_type yoffset) : x(xscale, xoffset), y(yscale, yoffset) {}
        constexpr udim2(const udim2& copy): x(copy.x), y(copy.y) {}

        template <typename TXS, typename TXO, typename TYS, typename TYO>
        constexpr udim2(TXS xscale, TXO xoffset, TYS yscale, TYO yoffset): x(xscale, xoffset), y(yscale, yoffset) {}

        constexpr vec2ut to_absolute(unit_type xrelative, unit_type yrelative) { return { x.to_absolute(xrelative), y.to_absolute(yrelative) }; }
        constexpr vec2ut to_absolute(vec2<unit_type> relative) { return udim2::to_absolute(relative.x, relative.y); }

        udim x;
        udim y;

        inline void setscale(unit_type x, unit_type y) { this->x.scale = x; this->y.scale = y; }
        inline void setscale(const vec2<unit_type>& vec) { return setscale(vec.x, vec.y); }

        inline void setoffset(unit_type x, unit_type y) { this->x.offset = x; this->y.offset = y; }
        inline void setoffset(const vec2<unit_type>& vec) { return setoffset(vec.x, vec.y); }

        static constexpr udim2 fromoffset(unit_type x, unit_type y) { return udim2{ 0, x, 0, y }; }
        static constexpr udim2 fromoffset(const vec2<unit_type>& vec) { return fromoffset(vec.x, vec.y); }

        static constexpr udim2 fromscale(unit_type x, unit_type y) { return udim2{ x, 0, y, 0 }; }
        static constexpr udim2 fromscale(const vec2<unit_type>& vec) { return fromscale(vec.x, vec.y); }

        static const udim2 ZERO;
        static const udim2 FULL;
    };
}