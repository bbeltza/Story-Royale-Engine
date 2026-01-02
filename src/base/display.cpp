#include <Base/Display.hpp>
#include "../internal.h"

sre::vec2ut sre::display_center() { return {engine.center_x, engine.center_y}; }
sre::vec2ut sre::display_size() { return {engine.size_x, engine.size_y}; }
sre::vec2i sre::display_outputsize() { return {engine.osize_x, engine.osize_y}; }
sre::unit sre::display_scale() { return engine.viewport_scale; }