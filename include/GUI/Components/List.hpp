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

        constexpr List(Direction dir=VERTICAL, sre::Alignment align=sre::A_LEFT, const sre::udim& padding=sre::UDIM_ZERO):
                    direction(dir),
                    alignment(align),
                    padding(padding) {}
    public:
        Direction direction;
        sre::Alignment alignment;
        sre::udim padding;
    protected:
        void process_children(const sre::rect2Dut& parent, sre::rect2Dut children[], size_t count) override;
    };
}

#endif