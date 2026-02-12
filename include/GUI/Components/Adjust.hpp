#ifndef SREGUI_ADJUST_HPP
#define SREGUI_ADJUST_HPP

#include <GUI/Component.hpp>

namespace sreGUI
{
    struct Transform;

    // Component that adjusts the size depending on the children
    // It may also optionally point to a transform component to determine the anchor point when rescaling
    // If it doesn't point to anything, then it will by default use [ 0 ; 0 ] as the anchor point
    struct Adjust: public Component
    {
        const Transform* transform_ptr = NULL;

    protected:
        void process_children(const sre::rect2Dut& parent, sre::rect2Dut children[], size_t count) override;
    };
}

#endif