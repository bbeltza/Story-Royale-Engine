#include "Components.h"

Game::Components::Shape::Shape() :
    xOffset(0),
    yOffset(0),
    Width(50),
    Height(50),
    shape(RECTANGLE),
    flags(VISIBLE)
{

}

bool Game::Components::Shape::isInScreenPoint(Vector2i pt)
{
    Entity* p = getParent();
    World* w = p->getWorld();

    static SDL_FPoint fpt;
    fpt.x = pt.X;
    fpt.y = pt.Y;

    switch (shape)
    {
    case RECTANGLE:
        return SDL_PointInFRect(&fpt, &m_renderRect);
        break;
    case CIRCLE:
        return (w->worldToScreenSpace(p->x + xOffset, p->y + yOffset) - pt).getMagnitude() <= Width/2;
        break;
    default:
        return 0;
        break;
    }
}