#include <Base/Display.hpp>
#include "../internal.h"
#include "../core/drivers/drivers.h"

sre::vec2ut sre::display_center() { return engine.video->center; }
sre::vec2ut sre::display_size() { return engine.video->size; }
sre::vec2i sre::display_outputsize() { return {engine.osize_x, engine.osize_y}; }
sre::unit sre::display_scale() { return engine.video->scale; }