#ifndef SREGUI_LIST_HPP
#define SREGUI_LIST_HPP

#include <GUI/Component.hpp>
#include <Datatypes/UDim.hpp>

#include <Base/Alignment.hpp>

namespace sreGUI
{
    struct List : public Component
    {
        enum Direction
        {
            HORIZONTAL,
            VERTICAL
        };
        Direction direction = VERTICAL;
        sre::Alignment alignment = sre::A_LEFT;

        sre::udim padding = sre::udim::ZERO;
    protected:
        void process_children(const sre::rect2Dut& parent, sre::rect2Dut children[], size_t count) override;
    };
}

#endif