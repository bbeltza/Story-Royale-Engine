#include <Core/Display.hpp>
#include <Core/Defer.hpp>
#include "../internal.h"
#include "drivers/drivers.h"


sre::vec2ut sre::display_center() { return engine.video->center; }
sre::vec2ut sre::display_size() { return engine.video->size; }
sre::vec2i sre::display_outputsize() { return {engine.osize_x, engine.osize_y}; }
sre::unit sre::display_scale() { return engine.video->scale; }

bool sre::display_setscale(int scale)
{
    if (!engine.video)
        return false;
    if (engine.auto_scalex || engine.auto_scaley)
        return false;

    engine.video->scale = static_cast<sre::unit>(scale);
    return true;
}

void sre::display_autoscale_on(int target_w, int target_h)
{
    assert((target_w && target_h) && "Use display_autoscale_off() instead, please");
    engine.auto_scalex = target_w;
    engine.auto_scaley = target_h;
}

void sre::display_autoscale_off()
{
    engine.auto_scalex = 0;
    engine.auto_scaley = 0;
}

void sre::display_vsync(bool enable)
{
    sre::defer<void>([](void* enable) { 
        engine.video->interface->vsync(engine.video, enable != NULL);
     }, enable ? reinterpret_cast<void*>(1) : NULL);
}