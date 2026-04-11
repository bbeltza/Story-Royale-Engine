#include <GUI/Components/Slice.hpp>
#include <Core/Render.h>

using namespace sreGUI;

void Slice::on_render(const sre::rect2Dut &dimensions, sre::RenderInterface* renderer)
{
    if (!texture)
        return;

    sre::vec2i size;
    renderer->sampler_query(texture, &size, NULL);

    sre::vec2f sizef{size};
    sre::rect2Df centerslice_uv{ center_slice.position / sizef, center_slice.size / sizef };

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

    sre::RenderInstance1 instances[9] = {
        { {dimensions.position, sre::vec2ut{center_slice.position}}, sre::vec2ut::ZERO, modulate, 0,  uvs[0], uvoffsets[0]},
        { {dimensions.position, sre::vec2ut{center_slice.position}}, sre::vec2ut::ZERO, modulate, 0,  uvs[1], uvoffsets[1]},
        { {dimensions.position, sre::vec2ut{center_slice.position}}, sre::vec2ut::ZERO, modulate, 0,  uvs[2], uvoffsets[2]},
        { {dimensions.position, sre::vec2ut{center_slice.position}}, sre::vec2ut::ZERO, modulate, 0,  uvs[3], uvoffsets[3]},
        { {dimensions.position, sre::vec2ut{center_slice.position}}, sre::vec2ut::ZERO, modulate, 0,  uvs[4], uvoffsets[4]},
        { {dimensions.position, sre::vec2ut{center_slice.position}}, sre::vec2ut::ZERO, modulate, 0,  uvs[5], uvoffsets[5]},
        { {dimensions.position, sre::vec2ut{center_slice.position}}, sre::vec2ut::ZERO, modulate, 0,  uvs[6], uvoffsets[6]},
        { {dimensions.position, sre::vec2ut{center_slice.position}}, sre::vec2ut::ZERO, modulate, 0,  uvs[7], uvoffsets[7]},
        { {dimensions.position, sre::vec2ut{center_slice.position}}, sre::vec2ut::ZERO, modulate, 0,  uvs[8], uvoffsets[8]}
    };

    renderer->draw1(0, instances, {texture});
}