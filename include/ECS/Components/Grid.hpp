#ifndef SREECS_TILESET_HPP
#define SREECS_TILESET_HPP

#include <ECS/Component.hpp>
#include <Base/Grid.hpp>

namespace sreECS
{
    // Customizable grid that draws (wraps sre::draw_grid)
    struct Grid: sreECS::Component
    {
        Grid(const sre::col4& col=sre::WHITE, const sre::vec2ut& size=16):
            color(col),
            size(size)
        {}
        sre::col4 color;
        sre::vec2ut size;

        void on_render(sreECS::Entity& entity) override
        {
            sre::draw_grid(color, size, entity.position, SRE_RENDERFLAGS_USECAM);
        };
    };
}

#endif