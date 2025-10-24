#pragma once
#include "Game/Component.hpp"

#include "Datatypes/Vector.hpp"
#include "Datatypes/Color.hpp"

#include "Classes/File.hpp"
#include "Classes/Texture.hpp"

namespace Components
{
    class Sprite : public ::Game::Component
    {       
    public:
        Sprite();
        ~Sprite();

        Vector2f Offset;
        Vector2f Scale;

        Color4 Modulate;

        size_t current_frame = 0;

        void Render(::Game::Entity*) override;

        void Attach(Texture&);
    private:
        std::vector<Texture*> textures;
    };
}