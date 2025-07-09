#include "Components.h"

Game::Components::Shape::Shape() :
    Rect(),
    shape(RECTANGLE),
    flags(VISIBLE)
{

}

bool Game::Components::Shape::isInScreenPoint(Vector2i pt)
{
    Entity* p = getParent();
    World* w = p->getWorld();

    static SDL_FPoint fpt;
    static SDL_FRect r;
    fpt.x = pt.X;
    fpt.y = pt.Y;

    static Vector2i screenSpace;

    switch (shape)
    {
    case RECTANGLE:
        screenSpace = w->worldToScreenSpace(p->x + Rect.getLeft(), p->y + Rect.getTop());
        r.x = screenSpace.X;
        r.y = screenSpace.Y;
        r.w = Rect.Size.X;
        r.h = Rect.Size.Y;
        return SDL_PointInFRect(&fpt, &r);
        break;
    case CIRCLE:
        screenSpace = w->worldToScreenSpace(p->x + Rect.Position.X, p->y + Rect.Position.Y);
        return (screenSpace - pt).getMagnitude() <= Rect.Size.X/2;
        break;
    default:
        return 0;
        break;
    }
}