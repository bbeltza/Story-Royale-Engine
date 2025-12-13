#pragma once
#include "Game/GuiComponent.hpp"
#include "datatypes/UDim.hpp"

namespace GuiComponents
{
    class Padding: public ::Game::GuiComponent
    {
        public:
        struct
        {
            UDim2 topleft = UDim2::ZERO;
            UDim2 bottomright = UDim2::ZERO;
        } padding;

        void setPadding(const UDim&);

    protected:
        void process_position(::Game::GuiContainer*) override;
        void pre_render(::Game::GuiContainer*) override;
    private:
        sre::rect2Dut m_oldabs;
    };
}