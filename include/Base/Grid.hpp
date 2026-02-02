#ifndef SRE_GRID_HPP
#define SRE_GRID_HPP

#include <Core/Draw.h>

namespace sre
{
    int draw_grid(const col4& color, const vec2ut& size, const vec2ut& origin, s32 flags);
}

#endif