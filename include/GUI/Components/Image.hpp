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
        Image(sre::col4 modulate=sre::WHITE, const sre::rect2Di& region={0, 0}):
            region(region), modulate(modulate) {}
        Image(const sre::RAIITexture& texture, sre::col4 modulate=sre::WHITE, const sre::rect2Di& region={0, 0}):
            texture(texture), region(region), modulate(modulate) {}
        Image(sre::RAIITexture&& texture, sre::col4 modulate=sre::WHITE, const sre::rect2Di& region={0, 0}):
            texture(std::move(texture)), region(region), modulate(modulate) {}

        sre::RAIITexture texture;
        sre::rect2Di region;
        sre::col4 modulate;

        bool fit(Transform& transform);
        void on_render(const sre::rect2Dut& dimensions) override;
    };
}

#endif