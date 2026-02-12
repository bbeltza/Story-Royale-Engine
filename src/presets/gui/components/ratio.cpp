#include <GUI/Components/Ratio.hpp>

using namespace sreGUI;

sre::vec2ut AspectRatio::process_size(const sre::rect2Dut& dimensions)
{
    return { dimensions.size.y * ratio, dimensions.size.y };
}