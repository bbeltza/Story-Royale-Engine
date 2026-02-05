#ifndef SRE_DISPLAY_HPP
#define SRE_DISPLAY_HPP
#include <Datatypes/Vector.hpp>

namespace sre
{
    sre::vec2ut display_size();
    sre::vec2ut display_center();
    sre::vec2i display_outputsize();

    sre::unit display_scale();

    bool display_setscale(int scale); // Set display scale
    void display_autoscale_on(int target_w, int target_h); // Set automatic scaling, targeting a resolution
    void display_autoscale_off(); // Disable automatic scaling

    void display_vsync(bool enable);
}

#endif