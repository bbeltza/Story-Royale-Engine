#pragma once
#include "Game/GuiComponent.hpp"
#include "datatypes/udim.hpp"

namespace GuiComponents
{
    class Padding: public ::Game::GuiComponent
    {
        public:
        struct
        {
            sre::udim2 topleft;
            sre::udim2 bottomright;
        } padding;

        void setPadding(const sre::udim&);

    protected:
        void process_position(::Game::GuiContainer*) override;
        void pre_render(::Game::GuiContainer*) override;
    private:
        sre::rect2Dut m_oldabs;
    };
}