#ifndef SREGUI_COMPONENT_HPP
#define SREGUI_COMPONENT_HPP

#include <Datatypes/Rect.hpp>

namespace sreGUI
{
    struct Component
    {
        friend class Object;
        bool enabled = true;
    protected:
        virtual void on_render(const sre::rect2Dut& dimensions) {}
        virtual void on_prerender(sre::rect2Dut& dimensions) {}
        virtual sre::vec2ut process_size(const sre::rect2Dut& dimensions) { return dimensions.size; }
        virtual sre::vec2ut process_position(const sre::rect2Dut& dimensions, const sre::vec2ut parent_size) { return dimensions.position; }
        virtual void process_children(const sre::rect2Dut& parent, sre::rect2Dut children[], size_t count) {}
    };
}

#endif