#ifndef SREGUI_LIST_HPP
#define SREGUI_LIST_HPP

#include <GUI/component.hpp>
#include "datatypes/udim.hpp"

namespace sreGUI
{
    struct List : public Component
    {
        enum Direction
        {
            HORIZONTAL,
            VERTICAL
        } direction = VERTICAL;
        sre::udim padding = sre::udim::ZERO;
    protected:
        void process_children(const sre::rect2Dut& parent, sre::rect2Dut children[], size_t count) override;

    };
}

#endif