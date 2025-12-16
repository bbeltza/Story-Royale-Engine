#pragma once
#include "Game/GuiComponent.hpp"
#include "datatypes/udim.hpp"

namespace GuiComponents
{
    class List : public ::Game::GuiComponent
    {
    public:
        enum Direction
        {
            HORIZONTAL,
            VERTICAL
        };

    public:
        Direction direction = VERTICAL;
        sre::udim Padding = sre::udim::ZERO;
    protected:
        void process_children(::Game::GuiObject*, uint32_t) override;

    private:
        sre::unit m_lastsize;
    };
}