#include <Core/Display.hpp>
#include <Core/Defer.hpp>
#include <Core/Render.h>
#include "../internal.h"


sre::vec2ut sre::display_center() { return { engine.vcenter_x, engine.vcenter_y}; }
sre::vec2ut sre::display_size() { return { engine.vsize_x, engine.vsize_y }; }
sre::vec2i sre::display_outputsize() { return { engine.osize_x, engine.osize_y }; }
sre::unit sre::display_scale() { return engine.scale; }

bool sre::display_setscale(int scale)
{
    if (!engine.video)
        return false;
    if (engine.auto_scalex || engine.auto_scaley)
        return false;

    engine.scale = static_cast<sre::unit>(scale);
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

namespace sre
{
    class CoreRenderer
    {
        public:
            static void vsync(bool enable) { engine.video->set_vsync(enable); }
    };
}

void sre::display_vsync(bool enable)
{
    sre_defer([](void* enable) { 
        CoreRenderer::vsync(enable != NULL);
     }, reinterpret_cast<void*>(enable));
}