#include <GUI/Components/fill.hpp>

#include "Base/Draw.hpp"

using namespace sreGUI;

void Fill::on_render(const sre::rect2Dut& dimensions)
{
    sre::draw(sre::DDRect{
        0,
        { r, g, b, a },

        dimensions.position.x,
        dimensions.position.y,
        dimensions.size.x,
        dimensions.size.y,
        0,
        0
    });
}