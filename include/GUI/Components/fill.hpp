#ifndef SREGUI_FILL_HPP
#define SREGUI_FILL_HPP

#include <GUI/component.hpp>
#include <datatypes/color.hpp>

namespace sreGUI
{
    struct Fill: public Component
    {
        sre::col4 color = sre::col4::WHITE;
    protected:
        void on_render(const sre::rect2Dut& dimensions) override;
    };
}

#endif