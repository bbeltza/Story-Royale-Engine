#ifndef SREGUI_SLICE_HPP
#define SREGUI_SLICE_HPP
#include <GUI/Components/Image.hpp>

namespace sreGUI
{
    struct Slice: public sreGUI::Image
    {
        sre::vec2i region_corners;
        protected:
        void on_render(const sre::rect2Dut &dimensions) override;
    };
}

#endif