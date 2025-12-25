#ifndef SREGUI_COMPONENT_HPP
#define SREGUI_COMPONENT_HPP

#include <datatypes/rect.hpp>

namespace sreGUI
{
    class Component
    {
        friend class Object;
    public:
        virtual ~Component() {}
    protected:
        virtual void on_render(Object& object) {}
        virtual void on_prerender(Object& object) {}
        virtual void process_size(Object& object, sre::vec2ut& size) {}
        virtual void process_position(Object& object, sre::vec2ut& size) {}
        virtual void process_children(Object& object, const sre::rect2Dut& parent, sre::rect2Dut& child, size_t index) {}
    };
}

#endif