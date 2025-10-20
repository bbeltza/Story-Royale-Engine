#include "Base/Input.hpp"

#include "Game/World.hpp"
#include "Game/Entity.hpp"
#include "Game/Component.hpp"

#include "Game/GuiLayer.hpp"
#include "Game/GuiObject.hpp"

#include "Game/Components/Shape.hpp"

static Game::GuiObject *queryObject = nullptr;
static Game::Entity *queryEntity = nullptr;

void queryObjects()
{
#if 0
    queryEntity = nullptr;
    queryObject = nullptr;

    SDL_FPoint pt{m_mouseState.x, m_mouseState.y};

    if (mlast_touchid < 0) goto callsection;
    int finger_count = SDL_GetNumTouchFingers(mlast_touchid);
    if (!finger_count) goto callsection;
    {
        Vector2f screensize = m_Engine->DrawingContext.getScreenSize();
        SDL_Finger* lastfinger = SDL_GetTouchFinger(mlast_touchid, finger_count - 1);

        pt.x = lastfinger->x * screensize.X;
        pt.y = lastfinger->y * screensize.Y;
    }


    callsection:

    queryObject = Game::GuiLayer::Current() ? Game::GuiLayer::Current()->_query(reinterpret_cast<float*>(&pt)) : 0;
    if (queryObject)
        return;
    queryEntity = Game::World::m_Current ? Game::World::m_Current->call_query(reinterpret_cast<float*>(&pt)) : 0;
#endif
}

bool Game::GuiObject::isHovering() const
{
    return queryObject == this;
}

// _query(float*) functions for both UI and World bases

Game::GuiObject *Game::GuiContainer::_query(float* pt)
{
    GuiObject *target_return = nullptr;

    for (auto it = m_children.rbegin(); it != m_children.rend(); it++)
    {
        auto obj = *it;
        if (!obj->canQuery)
            continue;
        target_return = obj->_query(pt);
        if (target_return) return target_return;
    }

    if (!isGuiLayer())
    {
        SDL_FRect* r{ reinterpret_cast<SDL_FRect*>(&m_absolute)};
        if (SDL_PointInFRect(reinterpret_cast<SDL_FPoint*>(pt), r))
            target_return = (GuiObject *)this;
    }

    return target_return;
}

bool Components::Shape::Query(Game::Entity *_entity, float* pt)
{
    return isInScreenPoint(_entity, {pt[0], pt[1]});
}