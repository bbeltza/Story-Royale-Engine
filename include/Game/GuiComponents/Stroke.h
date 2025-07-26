#pragma once
#include "Game/GuiComponent.h"

namespace GuiComponents
{
    class Stroke : public ::Game::GuiComponent
    {
    public:
        Stroke() { p_flags = RENDER; }

        unsigned int size = 1;
        Color4 color{0, 0, 0, 255};

    protected:
        void render(::Game::GuiContainer *) override;
    };
}