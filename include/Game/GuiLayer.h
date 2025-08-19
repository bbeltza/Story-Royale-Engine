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
        ~GuiLayer() { if (m_Current == this) m_Current = nullptr; }
    public:
        static inline GuiLayer* Current() {return m_Current;}
        static Color4 Foreground;
        
        bool isGuiLayer() const override { return 1; }

        template <class layer_t> static inline layer_t* setCurrent()
        {
            if (m_Current)
                delete m_Current;
            auto new_layer = new layer_t;
            m_Current = reinterpret_cast<GuiLayer*>(new_layer);
            return new_layer;
        }
    private:
        static GuiLayer* m_Current;
    };
}