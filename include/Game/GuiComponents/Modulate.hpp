#pragma once
#include "Game/GuiComponent.hpp"

namespace GuiComponents
{
    class Modulate: public ::Game::GuiComponent
    {
    public:
        sre::col4 Value = sre::col4::WHITE;
    protected:
        void pre_render(::Game::GuiContainer*) override;
    };
}