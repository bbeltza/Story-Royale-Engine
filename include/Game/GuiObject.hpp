#pragma once
#include "Game/GuiContainer.hpp"

#include "Datatypes/UDim.hpp"

namespace Game
{
    class GuiObject : public GuiContainer
    {
    public:
        UDim2 position, size;
        Vector2f anchor;

        bool isGuiLayer() const override { return 0; }
        bool isHovering() const;

    private:
        void _process();

        friend class GuiContainer;
    };
}