#ifndef SREGUI_FILL_HPP
#define SREGUI_FILL_HPP

#include <GUI/Component.hpp>
#include <Datatypes/Color.hpp>

namespace sreGUI
{
    struct Fill: public Component
    {
        template <typename... Args>
        Fill(Args&&... args): color(std::forward<Args>(args)...) {} // Build `Fill` instance with arguments passed as in `sre::col4`
        Fill(const sre::col4& col=sre::col4::WHITE): color(col) {}

        constexpr operator const sre::col4&() const { return color; }
        inline operator sre::col4&() { return color; }
    public:
        sre::col4 color;
    protected:
        void on_render(const sre::rect2Dut& dimensions) override;
    };
}

#endif