#ifndef SREGUI_BUTTON_HPP
#define SREGUI_BUTTON_HPP

#include <GUI/object.hpp>

#include <Base/Signal.hpp>
#include <Events/Mouse.hpp>
#include <Events/Touch.hpp>

namespace sreGUI
{
    class Button : public Object
    {
    public:
        Button();
        ~Button();

        virtual void on_press(sre::vec2ut pos) {}
        virtual void on_hover(bool hovered) {}
    private:
        static void mouse_click(void* SignalData, Button* button, const MouseButton* buttonData);
        static void finger_touch(void* SignalData, Button* button, const TouchFinger* mouseData);

        void update() override;

        bool m_hover = 0;
        ConnectionHandle m_click_event;
        ConnectionHandle m_touch_event;
    };
}

#endif