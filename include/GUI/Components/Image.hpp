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
        Image(sre::col4 modulate=sre::WHITE): modulate(modulate) {}
        Image(const sre::RAIITexture& texture, sre::col4 modulate=sre::WHITE): texture(texture), modulate(modulate) {}
        Image(sre::RAIITexture&& texture, sre::col4 modulate=sre::WHITE): texture(std::move(texture)), modulate(modulate) {}

        sre::RAIITexture texture;
        sre::col4 modulate;
        sre::rect2Di region{0, 0};

        void fit(Transform& transform);
        void on_render(const sre::rect2Dut& dimensions) override;
    };
}

#endif