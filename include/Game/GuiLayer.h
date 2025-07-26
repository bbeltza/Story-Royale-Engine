#pragma once
#include "Game/GuiContainer.h"

class EngineClass;

namespace Game
{
    class GuiLayer : public GuiContainer
    {
        friend class ::EngineClass;
    protected:
        GuiLayer() {}
    public:
        ~GuiLayer() {   if (Current == this) Current = nullptr;  }
        static GuiLayer *Current;
        static Color4 Foreground;
        
        bool isGuiLayer() const override { return 1; }

        template <class layer_t> static inline layer_t* setCurrent()
        {
            if (Current)
                delete Current;
            auto new_layer = new layer_t;
            Current = reinterpret_cast<GuiLayer*>(new_layer);
            return new_layer;
        }
    };
}