#pragma once
#include "Game/GuiComponent.hpp"
#include "Classes/Texture.hpp"

namespace GuiComponents
{
    class Image: public Game::GuiComponent
    {
        public:
        Image() { p_flags = RENDER; }
        Texture* CurrentImage = nullptr;
        Color4 Modulate = Color4::WHITE;

        protected:
        void render(Game::GuiContainer*) override;
    };
}