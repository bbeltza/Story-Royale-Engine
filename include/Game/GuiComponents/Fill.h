#pragma once
#include "Game/GuiComponent.h"

namespace GuiComponents
{
    class Fill: public ::Game::GuiComponent
    {
    public:
        Fill() {p_flags = RENDER;}

        Color4 color{255, 255, 255, 255};

    protected:
        void render(::Game::GuiContainer*) override;
    };
}