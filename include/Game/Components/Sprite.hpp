#pragma once
#include "Game/Component.hpp"

#include "datatypes/vector.hpp"
#include "datatypes/color.hpp"

#include "Base/File.hpp"
#include "Base/Texture.hpp"

namespace Components
{
    class Sprite : public ::Game::Component
    {       
    public:
        Sprite();
        ~Sprite();

        sre::vec2ut Scale;
        sre::vec2ut Offset;

        sre::col4 Modulate = sre::col4::WHITE;

        size_t current_frame = 0;

        void Render(::Game::Entity*) override;

        void Attach(Texture&);
    private:
        std::vector<Texture*> textures;
    };
}