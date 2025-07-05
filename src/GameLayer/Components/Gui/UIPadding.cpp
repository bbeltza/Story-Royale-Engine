#include "GuiComponents.h"

void Game::GuiComponents::UIPadding::setPadding(const UDim& value)
{
    padding.left.Offset = value.Offset;
    padding.left.Scale = value.Scale;

    padding.right.Offset = value.Offset;
    padding.right.Scale = value.Scale;

    padding.top.Offset = value.Offset;
    padding.top.Scale = value.Scale;

    padding.bottom.Offset = value.Offset;
    padding.bottom.Scale = value.Scale;
}

void Game::GuiComponents::UIPadding::process_position()
{
    SDL_FRect* absolute = getParentAbs();
    m_oldabs = *getParentAbs();

    const float l = padding.left.toAbsolute(absolute->w);
    const float t = padding.top.toAbsolute(absolute->h);
    const float r = padding.right.toAbsolute(absolute->w);
    const float b = padding.bottom.toAbsolute(absolute->h);


    absolute->x += l;
    absolute->y += t;
    absolute->w -= r + l;
    absolute->h -= b + t;
}

void Game::GuiComponents::UIPadding::pre_render()
{
    
    getParentAbs()->x = m_oldabs.x;
    getParentAbs()->x = m_oldabs.y;
    getParentAbs()->h = m_oldabs.h;
    getParentAbs()->w = m_oldabs.w;
}