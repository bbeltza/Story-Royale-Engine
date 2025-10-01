#pragma once
#include "Game/GuiComponent.hpp"
#include "Datatypes/UDim.hpp"

namespace GuiComponents
{
    class List : public ::Game::GuiComponent
    {
    public:
        enum Direction : char
        {
            dir_horizontal,
            dir_vertical
        };

    public:
        List() { p_flags = PROCESS_CHILDREN; }

        Direction direction = dir_vertical;
        UDim Padding = UDIM_ZERO;
    protected:
        void process_children(::Game::GuiObject*, uint32_t) override;

    private:
        float m_lastsize;
    };
}