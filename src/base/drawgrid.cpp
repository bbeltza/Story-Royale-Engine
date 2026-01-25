#include <Core/Draw.hpp>
#include <Core/Display.hpp>

#include <Base/Grid.hpp>

#include <ECS/scene.hpp>

int sre::draw_grid(const col4& color, const vec2ut& size, const vec2ut& origin, s32 flags)
{
    sre::DDLine line_data{
        flags & ~SRE_DRAWFLAGS_USECAM, // We got our own custom handler for that (For now...)
        color
    };

    const sre::vec2ut scr_size = sre::display_size();
    const sre::vec2ut scr_center = sre::display_center();
    
    const auto xgrids = static_cast<int>(ceil(scr_size.x / size.x / 2));
    const auto ygrids = static_cast<int>(ceil(scr_size.y / size.y / 2));

    const sre::vec2ut ceiled_scrsize = sre::vec2ut{ xgrids, ygrids } * size * 2;
    const sre::vec2ut ceiled_scrcenter = ceiled_scrsize / 2;;
    
    const sre::vec2ut scr_origin = flags & SRE_DRAWFLAGS_USECAM ? sreECS::World::current()->camera.toScreenSpace(origin) : origin;

    line_data.pt1.y = 0;
    line_data.pt2.y = scr_size.y;
    for (int i = -xgrids; i < xgrids; i++)
    {
        line_data.pt1.x = size.x * i + scr_origin.x;
        line_data.pt1.x = remainder(line_data.pt1.x, ceiled_scrsize.x) + ceiled_scrcenter.x;

        line_data.pt2.x = line_data.pt1.x;
        if (sre::draw(line_data))
            return -1;
    }

    line_data.pt1.x = 0;
    line_data.pt2.x = scr_size.x;
    for (int i = -ygrids; i < ygrids; i++)
    {
        line_data.pt1.y = size.y * i + scr_origin.y;
        line_data.pt1.y = remainder(line_data.pt1.y, ceiled_scrsize.y) + ceiled_scrcenter.y;

        line_data.pt2.y = line_data.pt1.y;
        if (sre::draw(line_data))
            return -1;
    }

    return 0;
}