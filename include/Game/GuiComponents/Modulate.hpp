#pragma once
#include "Game/GuiComponent.hpp"

namespace GuiComponents
{
    class Modulate: public ::Game::GuiComponent
    {
    public:
        Color4 Value{255, 255, 255, 255};
    protected:
        void pre_render(::Game::GuiContainer*) override;
    };
}