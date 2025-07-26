#pragma once
#include "Game/GuiLayer.h"
#include "Game/GuiComponents/Modulate.h"

#include "Classes/Signal.h"
#include "Events/Mouse.h"

namespace GuiPresets
{
    class Button : protected ::Game::GuiObject
    {
    public:
        Button();
        ~Button();

        virtual void ButtonClick(MouseButton *Event) {}
        virtual void MouseHover(bool hovered) {}

    private:
        static std::list<Button *> s_buttons;
        static void clickevent(MouseButton *buttonData);
        static Connection *s_clickconnection;

        void Update(float dt) override;

        bool m_hover = 0;
        GuiComponents::Modulate m_mod;
    };
}
