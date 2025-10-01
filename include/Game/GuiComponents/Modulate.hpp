#pragma once
#include "Game/GuiComponent.hpp"

namespace GuiComponents
{
    class Modulate: public ::Game::GuiComponent
    {
    public:
        Modulate() {p_flags = PRE_RENDER;}
        Color4 Value = {255, 255, 255, 255};
    protected:
        void pre_render(::Game::GuiContainer*) override;
    };
}