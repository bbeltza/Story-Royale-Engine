#include <Core/Render.h>

#include <Base/Grid.hpp>

#include <ECS/Scene.hpp>

#include <utils/mem.h>

bool sre::draw_grid(const col4& color, const vec2ut& size, vec2ut origin, s32 flags)
{
    if (!color.a)
        return false;
    
    if (!size.x || !size.y)
        return false;

    const sre::rect2Dut vp = sre::render::get_viewport_area();
    if (flags & SRE_DRAWFLAG_CAMERA) {
        sre::vec2ut cam = sre::render::get_viewport_center();
        // TODO: Add a sre::render::get_camera() function
            // This piece of code depends on sreECS, which is separate from this source file's dependency
            // It's fine if you don't use this function without sreECS, you won't get any linking errors, but if you do...
        if (sreECS::Scene* scn = sreECS::get_current())
            cam -= scn->camera.position;

        origin.x += cam.x * ((flags & SRE_DRAWFLAG_CAMERAX) != 0);
        origin.y += cam.y * ((flags & SRE_DRAWFLAG_CAMERAY) != 0);
    }
    
    // Only need the remainder between the absolute origin and the size
        // This will be the offset, all lines will be drawn ahead of this offset
        //   so this can be thought of the first line intersection point in the drawn grid
    origin.x = fmod(origin.x, size.x);
    origin.y = fmod(origin.y, size.y);

    
    // Make sure the remainder is positive, so it wraps to being between 0 and `size`, instead of between 0 and `-size` or `size` depending on the sign, when just calling `fmod()`
    origin.x += origin.x >= 0 ? 0 : size.x; 
    origin.y += origin.y >= 0 ? 0 : size.y; 

    const int numgridsx = (int)ceil(vp.size.x / size.x);
    const int numgridsy = (int)ceil(vp.size.y / size.y);

    const int pointcountx = numgridsx * 2;
    const int pointcounty = numgridsy * 2;
    const size_t pointcountxy = pointcountx + pointcounty;

    const sre::unit minx = 0;
    const sre::unit miny = 0;
    const sre::unit maxx = vp.size.x;
    const sre::unit maxy = vp.size.y;

    ut_dynsalloc(sre::RenderPoint, points, pointcountxy);
    sre::RenderPoint* const pointsx = points;
    sre::RenderPoint* const pointsy = points + pointcountx;

    for (int i = 0, d = 0; i < pointcountx; i += 2, d++) {
        auto& ptup = pointsx[i+0],
            & ptdown = pointsx[i+1];
        ptup = {
            { origin.x + d*size.x, miny },
            { 0 },
            { color }
        };
        ptdown = ptup;
        ptdown.pos.y =  maxy;
    }
    for (int i = 0, d = 0; i < pointcounty; i += 2, d++) {
        auto& ptleft = pointsy[i+0],
            & ptright = pointsy[i+1];
        ptleft = {
            { minx, origin.y + d*size.y },
            { 0 },
            { color }
        };
        ptright = ptleft;
        ptright.pos.x =  maxx;
    }

    const s32 renderflags = flags & ~SRE_DRAWFLAG_CAMERA;
    sre::render::draw2(renderflags, points, pointcountxy, SRE_PRIMITIVE_LINEPERLINE);

    // Draw a test red cross at the processed `origin` value
    #if 0
    sre::render::draw2(0, {
        { origin + sre::vec2ut{ 0, -3 }, 0, sre::RED },
        { origin + sre::vec2ut{ 0,  3 }, 0, sre::RED },

        { origin + sre::vec2ut{ -3,  0 }, 0, sre::RED },
        { origin + sre::vec2ut{  3,  0 }, 0, sre::RED }
    }, SRE_PRIMITIVE_LINEPERLINE);
    #endif
    return true;
}