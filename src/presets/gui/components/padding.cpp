#include <GUI/Components/padding.hpp>

using namespace sreGUI;

void Padding::set_all(const sre::udim& value)
{
    padding.top_left = {value, value};
    padding.bottom_right = {value, value};
}

sre::vec2ut Padding::process_position(const sre::rect2Dut& dimensions, sre::vec2ut)
{
    m_oldabs = dimensions;

    const sre::vec2ut lt = padding.top_left.to_absolute(dimensions.size);
    const sre::vec2ut rb = padding.bottom_right.to_absolute(dimensions.size);

    const_cast<sre::rect2Dut&>(dimensions).size -= lt + rb;
    return dimensions.position + lt;
}

void Padding::on_prerender(sre::rect2Dut& dimensions)
{
    dimensions = m_oldabs;
}