#pragma once
#include "Game/GuiComponent.hpp"

namespace GuiComponents
{
    class AspectRatio: public ::Game::GuiComponent
    {
    public:
        float ratio = 1;
    protected:
        void process_size(::Game::GuiContainer*) override;
    };
}