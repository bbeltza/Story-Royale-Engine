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
        ~GuiLayer() { if (m_Current == this) m_Current = nullptr; }
    public:
        template <class _layer=GuiLayer> static _layer* Current() { return dynamic_cast<_layer*>(m_Current); }
        static Color4 Foreground;
        
        bool isGuiLayer() const override { return 1; }

        template <class _layer=GuiObject, class... _args> static inline _layer* setCurrent(_args... args)
        {
            if (m_Current)
                m_Current->Destroy();
            auto new_layer = new _layer(args...);
            m_Current = new_layer;
            return new_layer;
        }
    private:
        static GuiLayer* m_Current;
    };
}