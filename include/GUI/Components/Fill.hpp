#ifndef SREGUI_FILL_HPP
#define SREGUI_FILL_HPP

#include <GUI/Component.hpp>
#include <Datatypes/Color.hpp>

namespace sreGUI
{
    struct Fill: public Component, sre::col4
    {
        Fill(const col4& col=WHITE): col4(col) {}
    protected:
        void on_render(const sre::rect2Dut& dimensions) override;
    };
}

#endif