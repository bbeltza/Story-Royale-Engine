#ifndef SREGUI_COMPONENT_HPP
#define SREGUI_COMPONENT_HPP

#include <Datatypes/Rect.hpp>
#include <Datatypes/Flags.hpp>

namespace sreGUI
{
    struct Component
    {
        friend class Object;

        enum flagHelpers
        {
            F_ENABLED = 1,
            F_FULL = -2 // Bitmask corresponding all of the free for use flags (it's 2^sizeof(void*) ^ 1, or just -2)
        };
        // Set of custom flags that you can set for the component
        // The first bit is reserved and must not be used for something else (it's the `F_ENABLED` constant)
        sre::flagsptr flags{F_ENABLED};

        // Enabling/disabling wrappers (it's fine to use the flags directly)

            inline bool enabled() const { return flags.has(F_ENABLED); }
            inline void enable() { flags.toggle_on(F_ENABLED); }
            inline void disable() { flags.toggle_off(F_ENABLED); }
        //

        // TODO too: Add custom constructor
    protected:
        virtual void on_render(const sre::rect2Dut& dimensions) {}
        virtual void on_prerender(sre::rect2Dut& dimensions) {}
        virtual sre::vec2ut process_size(const sre::rect2Dut& dimensions) { return dimensions.size; }
        virtual sre::vec2ut process_position(const sre::rect2Dut& dimensions, const sre::vec2ut parent_size) { return dimensions.position; }
        virtual void process_children(const sre::rect2Dut& parent, sre::rect2Dut children[], size_t count) {}
    };
}

#endif