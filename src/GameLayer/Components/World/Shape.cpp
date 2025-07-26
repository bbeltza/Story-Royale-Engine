#include "SDL.hpp"
#include "Game/Components/Shape.h"

#include "Game/Entity.h"
#include "Game/World.h"

Components::Shape::Shape() :
    Rect(),
    shape(RectangleShape),
    flags(VisibleFlag)
{
    p_flags = (ProcessFlags)(p_Render | p_pUpdate);
}

bool Components::Shape::isInScreenPoint(Game::Entity* p, Vector2f pt)
{
    auto w = p->getWorld<Game::World>();

    static SDL_FPoint fpt;
    static SDL_FRect r;
    fpt.x = pt.X;
    fpt.y = pt.Y;

    static Vector2f screenSpace;

    switch (shape)
    {
    case RectangleShape:
        screenSpace = w->worldToScreenSpace(p->Position.X + Rect.getLeft(), p->Position.Y + Rect.getTop());
        r.x = screenSpace.X;
        r.y = screenSpace.Y;
        r.w = Rect.Size.X;
        r.h = Rect.Size.Y;
        return SDL_PointInFRect(&fpt, &r);
        break;
    case CircleShape:
        screenSpace = w->worldToScreenSpace(p->Position.X + Rect.Position.X, p->Position.Y + Rect.Position.Y);
        return (screenSpace - pt).getMagnitude() <= Rect.Size.X/2;
        break;
    default:
        return 0;
        break;
    }
}