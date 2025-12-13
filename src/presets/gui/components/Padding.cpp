#include "Game/GuiComponents/Padding.hpp"

void GuiComponents::Padding::setPadding(const UDim& value)
{
    padding.topleft = {value, value};
    padding.bottomright = {value, value};
}

void GuiComponents::Padding::process_position(Game::GuiContainer* obj)
{
    auto absolute = getAbsolute(obj);
    m_oldabs = *absolute;

    const auto l = padding.topleft.X.toAbsolute(absolute->size.x);
    const auto t = padding.topleft.Y.toAbsolute(absolute->size.y);
    const auto r = padding.bottomright.X.toAbsolute(absolute->size.x);
    const auto b = padding.bottomright.Y.toAbsolute(absolute->size.y);

    absolute->position.x += l;
    absolute->position.y += t;
    absolute->size.x -= l + r;
    absolute->size.y -= t - b;
}

void GuiComponents::Padding::pre_render(Game::GuiContainer* obj)
{
    auto absolute = getAbsolute(obj);
    *absolute = m_oldabs;
}