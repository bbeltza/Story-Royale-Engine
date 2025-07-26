#pragma once
#include "Game/GuiComponent.h"
#include "Datatypes/UDim.h"

namespace GuiComponents
{
    class Padding: public ::Game::GuiComponent
    {
        public:
        Padding() { p_flags = PRE_RENDER | PROCESS_POSITION; }
        struct
        {
            UDim2 topleft = UDIM2_ZERO, bottomright = UDIM2_ZERO;
        } padding;

        void setPadding(const UDim&);

    protected:
        void process_position(::Game::GuiContainer*) override;
        void pre_render(::Game::GuiContainer*) override;
    private:
        RectF m_oldabs;
    };
}