#include "Base/Input.hpp"
#include "Game/GuiPresets/Button.hpp"

void GuiPresets::Button::clickevent(void *, Button *button, const MouseButton *buttonData)
{
    if (!buttonData->pressed)
        return;

    if (button->isHovering())
        button->ButtonClick(buttonData);
}

Connection *GuiPresets::Button::s_clickconnection = nullptr;

GuiPresets::Button::Button()
{
    m_connection = Input::MouseButton.Connect(clickevent, this);
    addComponent(m_mod);
}

GuiPresets::Button::~Button() {}

void GuiPresets::Button::Update(TimeStamp dt)
{
    m_hover = isHovering();
    m_mod.enabled = m_hover;
    if (!m_hover)
        return;

    if (Input::MouseButtonPressed())
        m_mod.Value = {150, 150, 150, 255};
    else
        m_mod.Value = {200, 200, 200, 255};
}