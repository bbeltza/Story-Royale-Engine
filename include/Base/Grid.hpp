#ifndef SRE_GRID_HPP
#define SRE_GRID_HPP

#include <Datatypes/Color.hpp>
#include <Datatypes/Vector.hpp>
#include <ints.h>

namespace sre
{
    bool draw_grid(const col4& color, const vec2ut& size, const vec2ut& origin, s32 flags);
}

#endif