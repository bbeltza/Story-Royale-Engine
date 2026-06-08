#ifndef SREGUI_IMAGE_HPP
#define SREGUI_IMAGE_HPP

#include <GUI/Component.hpp>
#include <Base/Texture.hpp>
#include <Datatypes/Color.h>

namespace sreGUI
{
    struct Transform;

    struct Image : public Component
    {
        enum Flags
        {
            F_REGION = ut_bit(1)
        };

        Image() = default;
        Image(const sre::RAIITexture& texture): texture(texture) {}
        Image(sre::RAIITexture&& texture): texture(std::move(texture)) {}

        sre::RAIITexture texture;
        sre::col4 modulate = sre::WHITE;
        sre::rect2Di region{0, 0};

        void fit(Transform& transform);
        void on_render(const sre::rect2Dut &dimensions) override;
    };
}

#endif