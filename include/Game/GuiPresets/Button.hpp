#pragma once
#include "Game/GuiLayer.hpp"
#include "Game/GuiComponents/Modulate.hpp"

#include "Base/Signal.hpp"
#include "Events/Mouse.hpp"
#include "Events/Touch.hpp"

namespace GuiPresets
{
    class Button : public ::Game::GuiObject
    {
    public:
        Button();
        ~Button();

        virtual void OnPress(sre::vec2ut pos) {}
        virtual void OnHover(bool hovered) {}

    private:
        static void mouse_click(void* SignalData, Button* button, const MouseButton* buttonData);
        static void finger_touch(void* SignalData, Button* button, const TouchFinger* mouseData);

        void update() override;

        bool m_hover = 0;
        ConnectionHandle m_click_event;
        ConnectionHandle m_touch_event;
    };
}
