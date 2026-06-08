#pragma once
#include <ECS/Component.hpp>

#include <Datatypes/Rect.h>
#include <Datatypes/Color.h>

#include <Base/File.hpp>
#include <Base/Texture.hpp>

namespace sreECS
{
    class Sprite : public Component
    {       
    public:
        Sprite(
            const sre::vec2ut& scale=sre::vec2ut::ONE,
            const sre::vec2ut& offset=sre::vec2ut::ZERO,
            const sre::col4& modulate=sre::WHITE
        ):
            scale(scale),
            offset(offset),
            modulate(modulate)
        {}

        sre::vec2ut scale;
        sre::vec2ut offset;

        sre::col4 modulate = sre::WHITE;
        sre::rect2Di region = { 0, 0 };

        sre::size_t current_frame = 0;

        void attach(sre::RAIITexture&&);
    private:
        void on_render(Entity&) override;

        std::vector<sre::RAIITexture> textures;
    };
}