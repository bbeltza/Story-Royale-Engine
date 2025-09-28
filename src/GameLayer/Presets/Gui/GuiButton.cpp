#include "Engine.h"
#include "Game/GuiPresets/Button.h"


void GuiPresets::Button::clickevent(void *, Button *button, MouseButton *buttonData)
{
    if (!buttonData->pressed)
        return;

    if (button->isHovering())
        button->ButtonClick(buttonData);
}

Connection *GuiPresets::Button::s_clickconnection = nullptr;

GuiPresets::Button::Button()
{
    m_connection = Engine->Input.mouseButton.Connect(clickevent, this);
    addComponent(&m_mod);
}

GuiPresets::Button::~Button()
{
    m_connection->Disconnect();
}

void GuiPresets::Button::Update(TimeStamp dt)
{
    m_hover = isHovering();
    m_mod.enabled = m_hover;
    if (!m_hover)
        return;

    if (Engine->Input.isMouseButtonPressed())
        m_mod.Value = {150, 150, 150, 255};
    else
        m_mod.Value = {200, 200, 200, 255};
}