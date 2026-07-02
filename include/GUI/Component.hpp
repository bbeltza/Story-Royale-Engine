#ifndef SREGUI_COMPONENT_HPP
#define SREGUI_COMPONENT_HPP

#include <Datatypes/Rect.h>
#include <Datatypes/Flags.hpp>

namespace sreGUI
{
    struct Component
    {
        friend class Object;
        enum StateBits {
            S_ENABLED = ut_bit(0)
        };

        // Custom flags (not managed internally)
        sre::flags32 flags{};

        inline void enable() { set_state(S_ENABLED, true); }
        inline void disable() { set_state(S_ENABLED, false); }
        constexpr bool enabled() const { return get_state(S_ENABLED); }

        constexpr Component() = default;
        constexpr Component(const sre::flags32& flags): flags(flags) {}
    protected:
        virtual sre::vec2ut process_size(const sre::rect2Dut& dimensions) { return dimensions.size; }
        virtual sre::vec2ut process_position(const sre::rect2Dut& dimensions, const sre::vec2ut& parent_size) { return dimensions.position; }
        virtual void process_children(const sre::rect2Dut& parent, sre::rect2Dut children[], size_t count) {}
        virtual void on_postprocess(sre::rect2Dut& dimensions) {}
        virtual void on_render(const sre::rect2Dut& dimensions) {}
    protected:
        inline void set_state(sre::u32 bits, bool enable) {
            if (enable)
                m_state.toggle_on(bits);
            else
                m_state.toggle_off(bits);
        }
        constexpr bool get_state(sre::u32 bits) const {
            return m_state.has(bits);
        }
    private:
        sre::flags32 m_state{S_ENABLED};
    };
}

#endif