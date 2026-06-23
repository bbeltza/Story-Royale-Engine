#include <Core/Render.h>

#include <Base/Grid.hpp>

#include <ECS/Scene.hpp>

#include <utils/mem.h>

bool sre::draw_grid(const col4& color, const vec2ut& size, const vec2ut& origin, s32 flags)
{
    const sre::rect2Dut vp = sre::render::get_viewport_area();
    //const sre::vec2ut scr_center = sre::display_center();
    
    const int numgridsx = (int)ceil(vp.size.x / size.x);
    const int numgridsy = (int)ceil(vp.size.y / size.y);

    const int pointcountx = numgridsx * 2;
    const int pointcounty = numgridsy * 2;

    const sre::unit minx = 0;
    const sre::unit miny = 0;
    const sre::unit maxx = vp.size.x;
    const sre::unit maxy = vp.size.y;

    ut_dynsalloc(sre::RenderPoint, points, ut_max(pointcountx, pointcounty));
    // Vertical lines (that depend on the X axis)
    for (int i = 0, ptindex = 0; i < numgridsx; i++, ptindex += 2)
    {
        sre::unit x = i * size.x + origin.x;
        points[ptindex+0] = {
            sre::vec2ut{x, miny},
            sre::vec2ut{0, 0},
            color
        };
        points[ptindex+1] = {
            sre::vec2ut{x, maxy},
            sre::vec2ut{0, 0},
            color
        };
    }
    sre::render::draw2(SRE_DRAWFLAG_CAMERAX, points, pointcountx, SRE_PRIMITIVE_LINEPERLINE);

    // Horizontal lines (that depend on the Y axis)
    for (int i = 0, ptindex = 0; i < numgridsy; i++, ptindex += 2)
    {
        sre::unit y = i * size.y + origin.y;
        points[ptindex+0] = {
            sre::vec2ut{minx, y},
            sre::vec2ut{0, 0},
            color
        };
        points[ptindex+1] = {
            sre::vec2ut{maxx, y},
            sre::vec2ut{0, 0},
            color
        };
    }
    sre::render::draw2(SRE_DRAWFLAG_CAMERAY, points, pointcounty, SRE_PRIMITIVE_LINEPERLINE);

    return true;
}