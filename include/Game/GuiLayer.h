#pragma once
#include "Game/GuiContainer.h"

class EngineClass;

namespace Game
{
    class GuiLayer: public GuiContainer
    {
        friend class ::EngineClass;

        GuiLayer() {}
        ~GuiLayer() { if (Current == this) Current = nullptr; }
        public:
        static GuiLayer* Current;
        static Color4 Foreground;

        bool isGuiLayer() const override {return 1;}
    };
}