#include "Engine.h"
#include "GuiPresets.h"

std::list<Game::GuiPresets::GuiButton*> Game::GuiPresets::GuiButton::s_buttons;
EVENT_CALLBACK(Game::GuiPresets::GuiButton::clickevent, MouseButton, buttonData,
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
)

Connection* Game::GuiPresets::GuiButton::s_clickconnection = nullptr;


Game::GuiPresets::GuiButton::GuiButton()
{
    if (!s_clickconnection) Engine->Input.mouseButton.Connect(clickevent);
    s_buttons.push_back(this);
}

Game::GuiPresets::GuiButton::~GuiButton()
{
    s_buttons.remove(this);
}

void Game::GuiPresets::GuiButton::Update(float dt)
{
    m_hover = isHovering();
    m_mod->enabled = m_hover;
    if (!m_hover) return;

    if (Engine->Input.isMouseButtonPressed())
        m_mod->Value = { 150, 150, 150, 255 };
    else
        m_mod->Value = { 200, 200, 200, 255 };

}