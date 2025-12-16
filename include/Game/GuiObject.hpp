#pragma once
#include "Game/GuiContainer.hpp"

#include "datatypes/udim.hpp"
#include "Base/Tween.hpp"

namespace Game
{
    class GuiObject : public GuiContainer
    {
    protected:
        ~GuiObject();
    public:
        sre::udim2 position;
        sre::udim2 size = sre::udim2::fromoffset(100, 100);
        sre::vec2f anchor;
        int zIndex = 0;

        bool isGuiLayer() const override { return 0; }
        bool isHovering() const;
        Signal<>* TweenPosition(const TweenInfo& Info, const sre::udim::unit_type* XScale, const sre::udim::unit_type* XOffset, const sre::udim::unit_type* YScale, const sre::udim::unit_type* YOffset);
        Signal<>* TweenSize(const TweenInfo& Info, const sre::udim::unit_type* XScale, const sre::udim::unit_type* XOffset, const sre::udim::unit_type* YScale, const sre::udim::unit_type* YOffset);
        Signal<>* TweenAnchor(const TweenInfo& Info, const float* X, const float* Y);

    private:
        void _process();
        static inline bool cmp(GuiObject* first, GuiObject* second) { return first->zIndex < second->zIndex; }

        friend class GuiContainer;

        TweenBase* tweens[10] = {nullptr};
    };
}