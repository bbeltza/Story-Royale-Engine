#include "Base/Input.hpp"
#include "Game/GuiPresets/Button.hpp"

using namespace GuiPresets;

// Event functions

void Button::mouse_click(void* _unused, Button* button, const MouseButton* buttonData)
{
    if (!buttonData->pressed)
        return;

    if (button->m_hover)
        button->OnPress(buttonData->position);
}

void Button::finger_touch(void* _unused, Button* button, const TouchFinger* touchData)
{
    if (touchData->Pressed)
        return;
    
    if (button->m_hover)
        button->OnPress(touchData->UV * Display::GetSize());
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
    m_hover = isHovering();
    if (last_hover != m_hover)
        this->OnHover(m_hover);
}