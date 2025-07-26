#pragma once
#include "Game/GuiComponent.h"

namespace GuiComponents
{
    class AspectRatio: public ::Game::GuiComponent
    {
    public:
        AspectRatio() { p_flags = PROCESS_SIZE; }
        float ratio = 1;
    protected:
        void process_size(::Game::GuiContainer*) override;
    };
}