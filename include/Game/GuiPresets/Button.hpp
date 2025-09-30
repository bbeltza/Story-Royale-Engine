#pragma once
#include "Game/GuiLayer.hpp"
#include "Game/GuiComponents/Modulate.hpp"

#include "Classes/Signal.hpp"
#include "Events/Mouse.hpp"

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
