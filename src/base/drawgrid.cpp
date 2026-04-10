#include <Core/Render.h>
#include <Core/Display.hpp>

#include <Base/Grid.hpp>

#include <ECS/Scene.hpp>

bool sre::draw_grid(const col4& color, const vec2ut& size, const vec2ut& origin, s32 flags)
{
    const sre::vec2ut scr_size = sre::display_size();
    //const sre::vec2ut scr_center = sre::display_center();
    
    const auto xgrids = static_cast<int>(ceil(scr_size.x / size.x / 2));
    const auto ygrids = static_cast<int>(ceil(scr_size.y / size.y / 2));

    const sre::vec2ut ceiled_scrsize = sre::vec2ut{ xgrids, ygrids } * size * 2;
    const sre::vec2ut ceiled_scrcenter = ceiled_scrsize / 2;;
    
    const sre::vec2ut scr_origin = flags & SRE_DRAWFLAGS_USECAM ? sreECS::World::current()->camera.toScreenSpace(origin) : origin;

    sre::vec2ut pt1;
    sre::vec2ut pt2;

    pt2.y = scr_size.y;
    for (int i = -xgrids; i < xgrids; i++)
    {
        pt1.x = size.x * i + scr_origin.x;
        pt1.x = remainder(pt1.x, ceiled_scrsize.x) + ceiled_scrcenter.x;

        pt2.x = pt1.x;
        if (!sre::draw_lines(color, false, pt1, pt2))
            return false;
    }

    pt1.x = 0;
    pt2.x = scr_size.x;
    for (int i = -ygrids; i < ygrids; i++)
    {
        pt1.y = size.y * i + scr_origin.y;
        pt1.y = remainder(pt1.y, ceiled_scrsize.y) + ceiled_scrcenter.y;

        pt2.y = pt1.y;
        if (!sre::draw_lines(color, false, pt1, pt2))
            return false;
    }

    return true;
}