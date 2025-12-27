#include <GUI/Components/fill.hpp>

#include "Base/Display.hpp"

using namespace sreGUI;

void Fill::on_render(const sre::rect2Dut& dimensions)
{
    Display::DrawRectangle(dimensions, *this, sre::vec2f::ZERO, Display::M_FILL, DISPLAY_DONT_CENTER);
}