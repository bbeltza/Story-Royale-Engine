#include <GUI/Components/Slice.hpp>
#include <Core/Render.h>

using namespace sreGUI;

void Slice::on_render(const sre::rect2Dut& dimensions)
{
    if (!texture)
        return;

    sre::vec2i size = texture->size();

    sre::vec2f sizef{size};
    sre::rect2Df centerslicef{center_slice};
    sre::rect2Df centerslice_uv{ centerslicef.position / sizef, centerslicef.size / sizef };

    sre::vec2f uvs[9] = {
        /* Left-top      */ {centerslice_uv.position.x, centerslice_uv.position.y},
        /* Middle-top    */ {centerslice_uv.size.x,     centerslice_uv.position.y},
        /* Right-top     */ {1 - centerslice_uv.size.x - centerslice_uv.position.x, centerslice_uv.position.y},
        /* Left-middle   */ {centerslice_uv.position.x, centerslice_uv.size.y},
        /* Center        */ {centerslice_uv.size.x, centerslice_uv.size.y},
        /* Right-middle  */ {1 - centerslice_uv.size.x - centerslice_uv.position.x, centerslice_uv.size.y},
        /* Left-bottom   */ {centerslice_uv.position.x, 1 - centerslice_uv.size.y - centerslice_uv.position.y},
        /* Middle-bottom */ {centerslice_uv.size.x, 1 - centerslice_uv.size.y - centerslice_uv.position.y},
        /* Right-bottom  */ {1 - centerslice_uv.size.x - centerslice_uv.position.x, 1 - centerslice_uv.size.y - centerslice_uv.position.y}
    };
    sre::vec2f uvoffsets[9] = {
        {0, 0},
        {centerslice_uv.position.x, 0},
        {centerslice_uv.position.x + centerslice_uv.size.x, 0},
        {0, centerslice_uv.position.y},
        {centerslice_uv.position.x, centerslice_uv.position.y},
        {centerslice_uv.position.x + centerslice_uv.size.x, centerslice_uv.position.y},
        {0, centerslice_uv.position.y + centerslice_uv.size.y},
        {centerslice_uv.position.x, centerslice_uv.position.y + centerslice_uv.size.y},
        {centerslice_uv.position.x + centerslice_uv.size.x, centerslice_uv.position.y + centerslice_uv.size.y}
    };

    sre::rect2Dut rects[9] = {
        { { dimensions.position.x, dimensions.position.y },
            { centerslicef.position.x, centerslicef.position.y } },
        { { dimensions.position.x + centerslicef.position.x, rects[0].position.y },
            { dimensions.size.x - sizef.x + centerslicef.size.y, rects[0].size.y } },
        { { rects[1].position.x + rects[1].size.x, rects[0].position.y },
            { sizef.x - centerslicef.position.x - centerslicef.size.x, rects[0].size.y } },

        { { rects[0].position.x, dimensions.position.y + centerslicef.position.y },
            { rects[0].size.x, dimensions.size.y - sizef.y + centerslicef.size.y } },
        { { rects[1].position.x, rects[3].position.y },
            { rects[1].size.x, rects[3].size.y } },
        { { rects[2].position.x, rects[3].position.y },
            { rects[2].size.x, rects[3].size.y } },

        { { rects[0].position.x, rects[3].position.y + rects[3].size.y },
            { rects[0].size.x, sizef.y - centerslicef.position.y - centerslicef.size.y } },
        { { rects[1].position.x, rects[6].position.y },
            { rects[1].size.x, rects[6].size.y } },
        { { rects[2].position.x, rects[6].position.y },
            { rects[2].size.x, rects[6].size.y } }
    };

    sre::RenderInstance1 instances[9] = { // Doesn't work for now, `rect` has to be set
        { rects[0], sre::vec2ut::ZERO, modulate, 0,  uvs[0], uvoffsets[0]},
        { rects[1], sre::vec2ut::ZERO, modulate, 0,  uvs[1], uvoffsets[1]},
        { rects[2], sre::vec2ut::ZERO, modulate, 0,  uvs[2], uvoffsets[2]},
        { rects[3], sre::vec2ut::ZERO, modulate, 0,  uvs[3], uvoffsets[3]},
        { rects[4], sre::vec2ut::ZERO, modulate, 0,  uvs[4], uvoffsets[4]},
        { rects[5], sre::vec2ut::ZERO, modulate, 0,  uvs[5], uvoffsets[5]},
        { rects[6], sre::vec2ut::ZERO, modulate, 0,  uvs[6], uvoffsets[6]},
        { rects[7], sre::vec2ut::ZERO, modulate, 0,  uvs[7], uvoffsets[7]},
        { rects[8], sre::vec2ut::ZERO, modulate, 0,  uvs[8], uvoffsets[8]}
    };

    sre::render::draw1(0, instances, texture.get());
}