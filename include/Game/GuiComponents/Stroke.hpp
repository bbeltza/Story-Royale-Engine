#pragma once
#include "Game/GuiComponent.hpp"

namespace GuiComponents
{
    class Stroke : public ::Game::GuiComponent
    {
    public:
        unsigned int size = 1;
        sre::col4 color = sre::col4::BLACK;

    protected:
        void render(::Game::GuiContainer *) override;
    };
}