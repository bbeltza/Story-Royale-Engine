#pragma once
#include "Game/GuiContainer.hpp"

class EngineClass;

namespace Game
{
    class GuiLayer : public GuiContainer
    {
        friend class ::EngineClass;
    protected:
        GuiLayer() {}
        ~GuiLayer();
    public:
        template <class _layer=GuiLayer> static _layer* Current() { return dynamic_cast<_layer*>(curr()); }
        Color4 Foreground = { 0, 0, 0, 0 };
        
        bool isGuiLayer() const override { return 1; }

        template <class _layer=GuiObject, class... _args> static inline _layer* setCurrent(_args... args)
        {
            auto new_layer = new _layer(args...);
            set(new_layer);
            return new_layer;
        }
    private:
        static void set(GuiLayer* layer);
        static GuiLayer* curr();
    };
}