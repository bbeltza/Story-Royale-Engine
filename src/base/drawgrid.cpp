#include <Core/Render.h>
#include <Core/Display.hpp>

#include <Base/Grid.hpp>

#include <ECS/Scene.hpp>

#include <utils/mem.h>

bool sre::draw_grid(const col4& color, const vec2ut& size, const vec2ut& origin, s32 flags)
{
    const sre::vec2ut scr_size = sre::display_size();
    //const sre::vec2ut scr_center = sre::display_center();
    
    int numgridsx = (int)ceil(scr_size.x / size.x);
    int numgridsy = (int)ceil(scr_size.y / size.y);

    ut_dynsalloc(sre::vec2ut, points, numgridsx + numgridsy);

    sre::render_draw2(
        0, color, points, numgridsx + numgridsy
    );

    return true;
}