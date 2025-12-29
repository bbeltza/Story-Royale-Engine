#ifndef SREGUI_IMAGE_HPP
#define SREGUI_IMAGE_HPP

#include <GUI/component.hpp>
#include <Base/Texture.hpp>

namespace sreGUI
{
    struct Transform;

    struct Image : public Component
    {
        Texture* texture = NULL;
        sre::col4 modulate = sre::col4::WHITE;

        void fit(Transform& transform);
    protected:
        void on_render(const sre::rect2Dut &dimensions) override;
    };
}

#endif