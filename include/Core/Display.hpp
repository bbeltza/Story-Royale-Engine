#ifndef SRE_DISPLAY_HPP
#define SRE_DISPLAY_HPP
#include <Datatypes/Vector.hpp>

namespace sre
{
    sre::vec2ut display_size();
    sre::vec2ut display_center();
    sre::vec2i display_outputsize();

    sre::unit display_scale();
}

#endif