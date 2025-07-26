#pragma once
#include "Game/GuiContainer.h"

#include "Datatypes/Udim.h"

namespace Game
{
    class GuiObject : public GuiContainer
    {
    public:
        GuiObject();

        UDim2 position, size;
        Vector2f anchor;

        bool isGuiLayer() const override { return 0; }
        bool isHovering() const;

    private:
        void _process();

        friend class GuiContainer;
    };
}