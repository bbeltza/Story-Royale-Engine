#include <GUI/Components/ratio.hpp>

using namespace sreGUI;

sre::vec2ut AspectRatio::process_size(const sre::rect2Dut& dimensions)
{
    return { dimensions.size.y * ratio, dimensions.size.y };
}