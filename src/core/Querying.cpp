#include "../internal.h"

#include "Base/Input.hpp"

#include "Game/World.hpp"
#include "Game/Entity.hpp"
#include "Game/Component.hpp"

#include "Game/GuiLayer.hpp"
#include "Game/GuiObject.hpp"

#include "Game/Components/Shape.hpp"

static Game::GuiObject *queryObject = nullptr;
static Game::Entity *queryEntity = nullptr;

void __query_objects()
{
    queryEntity = nullptr;
    queryObject = nullptr;

    SDL_FPoint pt{engine.mouse_x, engine.mouse_y};

    if (engine.input_last_touchid < 0) goto callsection;
    {
        int finger_count = SDL_GetNumTouchFingers(engine.input_last_touchid);
        if (!finger_count) goto callsection;

        sre::vec2f screensize{ engine.viewport.w, engine.viewport.h };
        SDL_Finger* lastfinger = SDL_GetTouchFinger(engine.input_last_touchid, finger_count - 1);

        pt.x = lastfinger->x * screensize.x;
        pt.y = lastfinger->y * screensize.y;
    }


    callsection:

    queryObject = engine.current_guilayer ? currlayer->_query(reinterpret_cast<float*>(&pt)) : 0;
    if (queryObject)
        return;
    queryEntity = engine.current_world ? currworld->call_query(reinterpret_cast<float*>(&pt)) : 0;
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