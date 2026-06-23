#ifndef SREGUI_BUTTON_HPP
#define SREGUI_BUTTON_HPP

#include <GUI/Object.hpp>

#include <Core/Event.hpp>

namespace sreGUI
{
    class Button : public Object
    {
    public:
        Button(sreGUI::Object* parent=NULL);
        ~Button();

        virtual void on_press(sre::vec2ut pos) {}
        virtual void on_hover(bool hovered) {}
    private:
        static void handle_event(void* signalData, Button* button, sre::Event eventData);

        void update() override;

        bool m_hover = 0;
        sre::Connection m_event;
    };
}

#endif