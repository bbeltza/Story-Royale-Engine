#ifndef SREGUI_PADDING_HPP
#define SREGUI_PADDING_HPP

#include <GUI/Component.hpp>
#include "Datatypes/UDim.hpp"

namespace sreGUI
{
    struct Padding: public Component
    {
        struct {
            sre::udim2 top_left;
            sre::udim2 bottom_right;
        } padding;

        void set_all(const sre::udim& padding);
    protected:
        sre::vec2ut process_position(const sre::rect2Dut& dimensions, const sre::vec2ut& parent) override;
        void on_postprocess(sre::rect2Dut& dimensions) override;
    private:
        sre::rect2Dut m_oldabs;
    };
}

#endif