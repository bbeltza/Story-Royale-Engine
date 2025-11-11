#pragma once
#include "Game/GuiComponent.hpp"
#include "Base/Texture.hpp"

namespace GuiComponents
{
    class Image: public Game::GuiComponent
    {
        public:
        Texture* CurrentImage = nullptr;
        Color4 Modulate = Color4::WHITE;

        void Fit(Game::GuiObject* obj);
        protected:
        void render(Game::GuiContainer*) override;
    };
}