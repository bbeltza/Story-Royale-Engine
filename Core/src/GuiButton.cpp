#include "Input.h"
#include "GuiPresets.h"

std::vector<Game::GuiPresets::GuiButton*> Game::GuiPresets::GuiButton::s_buttons;
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
    if (!s_clickconnection) Engine::Input::mouseButton.Connect(clickevent);
    s_buttons.push_back(this);
}

Game::GuiPresets::GuiButton::~GuiButton()
{
    for (int i = 0; i < s_buttons.size(); i++)
    {
        if (s_buttons[i] == this)
        {
            s_buttons.erase(s_buttons.begin() + i);
            break;
        }
    }
}

void Game::GuiPresets::GuiButton::Update(float dt)
{
    m_hover = isHovering();
    if (m_hover)
    {
        
    }
}