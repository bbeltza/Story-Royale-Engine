#include "Game/GuiComponents/Padding.hpp"

void GuiComponents::Padding::setPadding(const sre::udim& value)
{
    padding.topleft = {value, value};
    padding.bottomright = {value, value};
}

void GuiComponents::Padding::process_position(Game::GuiContainer* obj)
{
    auto absolute = getAbsolute(obj);
    m_oldabs = *absolute;

    const sre::vec2ut lt = padding.topleft.to_absolute(absolute->size);
    const sre::vec2ut rb = padding.bottomright.to_absolute(absolute->size);

    absolute->position += lt;
    absolute->size -= lt + rb;
}

void GuiComponents::Padding::pre_render(Game::GuiContainer* obj)
{
    auto absolute = getAbsolute(obj);
    *absolute = m_oldabs;
}