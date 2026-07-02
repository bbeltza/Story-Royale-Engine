#ifndef SREGUI_TRANSFORM_HPP
#define SREGUI_TRANSFORM_HPP

#include <GUI/Component.hpp>
#include <Datatypes/UDim.hpp>

namespace sreGUI
{
    struct Transform: public Component
    {
        constexpr Transform() = default;
        constexpr Transform(const sre::udim2& position, const sre::udim2& size, const sre::vec2ut& anchor = sre::vec2ut::ZERO): position(position), size(size), anchor(anchor) {}

        sre::udim2 position;
        sre::udim2 size;
        sre::vec2ut anchor;

        sre::vec2ut process_size(const sre::rect2Dut& dimensions) override {
            return size.to_absolute(dimensions.size);
        }
        sre::vec2ut process_position(const sre::rect2Dut& dimensions, const sre::vec2ut& parent_size) override {
            return dimensions.position + position.to_absolute(parent_size) - dimensions.size * anchor;
        }
    };
}

#endif