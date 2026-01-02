#include <GUI/Presets/button.hpp>

#include <Base/Input.hpp>
#include <Base/Display.hpp>

using namespace sreGUI;

// Event functions

void Button::mouse_click(void* _unused, Button* button, const MouseButton* buttonData)
{
    if (!buttonData->pressed)
        return;

    if (button->m_hover)
        button->on_press(buttonData->position);
}

void Button::finger_touch(void* _unused, Button* button, const TouchFinger* touchData)
{
    if (touchData->Pressed)
        return;
    
    if (button->m_hover)
        button->on_press(touchData->UV * sre::display_size());
}


// Class methods

Button::Button():
    m_click_event(Input::MouseButton.Connect(mouse_click, this)),
    m_touch_event(Input::FingerTouch.Connect(finger_touch, this))
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