#ifndef SREGUI_RATIO_HPP
#define SREGUI_RATIO_HPP

#include <GUI/component.hpp>

namespace sreGUI
{
    struct AspectRatio: public Component
    {
        sre::unit ratio = 1;
    protected:
        sre::vec2ut process_size(const sre::rect2Dut& dimensions) override;
    };
}

#endif