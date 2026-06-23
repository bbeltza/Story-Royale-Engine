#include <GUI/Presets/Button.hpp>

#include <Core/Event.hpp>
#include <Core/Window.hpp>

using namespace sreGUI;

void Button::handle_event(void*, Button* button, sre::Event ev)
{
    if (!button->flags.has(F_ENABLED)) return;

    switch (ev.type())
    {
    using namespace sre::events;
    case sre::EVENT_MOUSEBUTTON: {
        const MouseButton& mev = ev;
        if (!mev.pressed) return;
        if (button->m_hover) button->on_press(mev.process_position());
    } break;
    case sre::EVENT_TOUCH: {
        const Touch& tev = ev;
        if (tev.pressed) return;
        if (button->m_hover) button->on_press(tev.process_position());
    } break;
    default:
        break;
    }
}

Button::Button(sreGUI::Object* parent):
    Object(parent),
    m_event(sre::onEvent.connect(handle_event, this))
{

}

Button::~Button() {}

void Button::update()
{
    bool last_hover = m_hover;
    m_hover = is_hovering();
    if (last_hover != m_hover)
        this->on_hover(m_hover);
}