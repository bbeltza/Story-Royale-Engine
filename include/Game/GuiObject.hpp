#pragma once
#include "Game/GuiContainer.hpp"

#include "Datatypes/UDim.hpp"
#include "Base/Tween.hpp"

namespace Game
{
    class GuiObject : public GuiContainer
    {
    protected:
        ~GuiObject();
    public:
        UDim2 position; 
        UDim2 size{0, 100, 0, 100};
        Vector2f anchor;
        int zIndex = 0;

        bool isGuiLayer() const override { return 0; }
        bool isHovering() const;
        Signal<>* TweenPosition(TweenInfo& Info, const float* XScale, const int* XOffset, const float* YScale, const int* YOffset);
        Signal<>* TweenSize(TweenInfo& Info, const float* XScale, const int* XOffset, const float* YScale, const int* YOffset);
        Signal<>* TweenAnchor(TweenInfo& Info, const float* X, const float* Y);

    private:
        void _process();
        static inline bool cmp(GuiObject* first, GuiObject* second) { return first->zIndex < second->zIndex; }

        friend class GuiContainer;

        TweenBase* tweens[10] = {nullptr};
    };
}