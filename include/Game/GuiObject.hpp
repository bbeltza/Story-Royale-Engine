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
        int zIndex = 0;

        bool isGuiLayer() const override { return 0; }
        bool isHovering() const;

    private:
        void _process();
        static inline bool cmp(GuiObject* first, GuiObject* second)
        {
            return first->zIndex < second->zIndex;
        }

        friend class GuiContainer;
    };
}