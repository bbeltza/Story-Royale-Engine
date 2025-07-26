#include "Game/GuiComponents/Padding.h"

void GuiComponents::Padding::setPadding(const UDim& value)
{
    padding.topleft = {value, value};
    padding.bottomright = {value, value};
}

void GuiComponents::Padding::process_position(Game::GuiContainer* obj)
{
    auto absolute = getAbsolute(obj);
    m_oldabs = *absolute;

    const float l = padding.topleft.X.toAbsolute(absolute->Size.X);
    const float t = padding.topleft.Y.toAbsolute(absolute->Size.Y);
    const float r = padding.bottomright.X.toAbsolute(absolute->Size.X);
    const float b = padding.bottomright.Y.toAbsolute(absolute->Size.Y);

    absolute->Position.X += l;
    absolute->Position.Y += t;
    absolute->Size.X -= l + r;
    absolute->Size.Y -= t - b;
}

void GuiComponents::Padding::pre_render(Game::GuiContainer* obj)
{
    auto absolute = getAbsolute(obj);
    *absolute = m_oldabs;
}