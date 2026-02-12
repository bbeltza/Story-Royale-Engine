#include <GUI/Components/Fill.hpp>

#include <Core/Draw.hpp>

using namespace sreGUI;

void Fill::on_render(const sre::rect2Dut& dimensions)
{
    sre::draw(sre::DDRect{
        0,
        *this,
        dimensions,
        sre::vec2ut::ZERO
    });
}