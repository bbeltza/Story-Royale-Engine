#include "Engine.h"
#include "Game/GuiPresets/Button.h"

std::list<GuiPresets::Button*> GuiPresets::Button::s_buttons;

void GuiPresets::Button::clickevent(MouseButton* buttonData)
{
    if (!buttonData->pressed) return;
    for (auto button : s_buttons)
    {
        if (!button->canQuery) continue;
        if (button->isHovering())
        {
            button->ButtonClick(buttonData);
            break;
        }
    }
}

Connection* GuiPresets::Button::s_clickconnection = nullptr;

GuiPresets::Button::Button()
{
    if (!s_clickconnection) Engine->Input.mouseButton.Connect(event_callback(clickevent));
    s_buttons.push_back(this);
    addComponent(&m_mod);
}

GuiPresets::Button::~Button()
{
    s_buttons.remove(this);
}

void GuiPresets::Button::Update(float dt)
{
    m_hover = isHovering();
    m_mod.enabled = m_hover;
    if (!m_hover) return;

    if (Engine->Input.isMouseButtonPressed())
        m_mod.Value = { 150, 150, 150, 255 };
    else
        m_mod.Value = { 200, 200, 200, 255 };

}