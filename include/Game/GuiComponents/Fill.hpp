#pragma once
#include "Game/GuiComponent.hpp"

namespace GuiComponents
{
    class Fill: public ::Game::GuiComponent
    {
    public:
        sre::col4 color = sre::col4::WHITE;
    protected:
        void render(::Game::GuiContainer*) override;
    };
}