#ifndef SREGUI_TRANSFORM_HPP
#define SREGUI_TRANSFORM_HPP

#include <GUI/component.hpp>
#include <datatypes/udim.hpp>

namespace sreGUI
{
    struct Transform: public Component
    {
        Transform() = default;
        Transform(const sre::udim2& position, const sre::udim2& size, const sre::vec2ut& anchor = sre::vec2ut::ZERO): position(position), size(size), anchor(anchor) {}

        sre::udim2 position;
        sre::udim2 size;
        sre::vec2ut anchor;
    protected:
        sre::vec2ut process_size(const sre::rect2Dut& dimensions) override;
        sre::vec2ut process_position(const sre::rect2Dut& dimensions, sre::vec2ut parent_size) override;
    };
}

#endif