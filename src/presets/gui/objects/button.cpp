#include <GUI/Presets/Button.hpp>

#include <Core/Event.hpp>
#include <Core/Display.hpp>

using namespace sreGUI;

void Button::handle_event(void*, Button* button, sre::Event ev)
{
    if (!button->flags.has(F_ENABLED)) return;

    switch (ev.type)
    {
    case sre::EVENT_MOUSEBUTTON:
        if (!ev.mouse_button.pressed) return;
        if (button->m_hover) button->on_press(ev.mouse_button.position);
        break;
    case sre::EVENT_TOUCH:
        if (ev.touch.pressed) return;
        if (button->m_hover) button->on_press(ev.touch.uv * sre::display_size());
        break;
    default:
        break;
    }
}

Button::Button():
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