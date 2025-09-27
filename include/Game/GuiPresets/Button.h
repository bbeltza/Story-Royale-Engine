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
        static void clickevent(void*, Button* button, MouseButton *buttonData);
        static Connection *s_clickconnection;

        void Update(TimeStamp dt) override;

        bool m_hover = 0;
        Connection* m_connection;
        GuiComponents::Modulate m_mod;
    };
}
