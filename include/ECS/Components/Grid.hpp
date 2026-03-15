#ifndef SREECS_TILESET_HPP
#define SREECS_TILESET_HPP

#include <ECS/Component.hpp>
#include <Core/Draw.hpp>
#include <Base/Grid.hpp>

namespace sreECS
{
    // Customizable grid that draws (wraps sre::draw_grid)
    struct Grid: sreECS::Component
    {
        enum Flags
        {
            F_RESERVED = F_ENABLED,
            F_DRAWCENTER = ut_bit(1)
        };

        Grid(const sre::col4& col=sre::WHITE, const sre::vec2ut& size=16):
            color(col),
            size(size)
        {}
        sre::col4 color;
        sre::vec2ut size;

        void on_render(sreECS::Entity& entity) override
        {
            sre::draw_grid(color, size, entity.position, SRE_DRAWFLAGS_USECAM);
            if (flags.has(F_DRAWCENTER))
                sre::draw(sre::DDRect{
                    SRE_DRAWFLAGS_USECAM,
                    sre::col4{color, 255},

                    {entity.position, size/4},
                    sre::vec2ut::CENTER
                });
        };
    };
}

#endif