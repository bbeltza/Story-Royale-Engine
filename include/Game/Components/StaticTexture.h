#pragma once
#include "Game/Component.h"

#include "Datatypes/Vector.h"

struct SDL_Texture;

namespace Components
{
    class StaticTexture: public ::Game::Component
    {
        public:
        StaticTexture();
        ~StaticTexture();

        void Render(::Game::Entity*) override;

        void AddFile(const char*, std::list<Vector2f>&);

        Vector2i getEntireSize() const;
        Vector2f Offset{0, 0};

        private:

        struct final_texturedata
        {
                SDL_Texture* texture;
                Vector2f offset;
                final_texturedata(SDL_Texture* t, float x, float y):texture(t), offset(x, y) {}
        };
        std::list<final_texturedata> m_FinalTextures;    
    };
}