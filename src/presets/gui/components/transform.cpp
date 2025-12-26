#include <GUI/Components/transform.hpp>

using namespace sreGUI;

sre::vec2ut Transform::process_size(const sre::rect2Dut& dimensions) { return size.to_absolute(dimensions.size); }

sre::vec2ut Transform::process_position(const sre::rect2Dut& dimensions, sre::vec2ut parent_size) { return dimensions.position + position.to_absolute(parent_size) - dimensions.size * anchor; }