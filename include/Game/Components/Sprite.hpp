#pragma once
#include "Game/Component.hpp"

#include "Datatypes/Vector.hpp"
#include "Datatypes/Color.hpp"

#include "Base/File.hpp"
#include "Base/Texture.hpp"

namespace Components
{
    class Sprite : public ::Game::Component
    {       
    public:
        Sprite();
        ~Sprite();

        Vector2ut Scale;
        Vector2ut Offset;

        Color4 Modulate = Color4::WHITE;

        size_t current_frame = 0;

        void Render(::Game::Entity*) override;

        void Attach(Texture&);
    private:
        std::vector<Texture*> textures;
    };
}