#include "../internal.h"

#include "Base/Input.hpp"
#include "Base/Display.hpp"

#include "ECS/scene.hpp"
#include "ECS/entity.hpp"

#include "GUI/object.hpp"

static const sreGUI::Object *queryObject = nullptr;
static const sreECS::Entity *queryEntity = nullptr;

void __query_objects()
{
    // queryEntity = nullptr;
    // queryObject = nullptr;

    sre::vec2ut pt{engine.mouse_x, engine.mouse_y};

    if (engine.input_last_touchid < 0) goto callsection;
    {
        int finger_count = SDL_GetNumTouchFingers(engine.input_last_touchid);
        if (!finger_count) goto callsection;

        
        SDL_Finger* lastfinger = SDL_GetTouchFinger(engine.input_last_touchid, finger_count - 1);

        pt = sre::display_size() * sre::vec2ut{ lastfinger->x, lastfinger->y };
    }


    callsection:

    queryObject = engine.current_guilayer ? currlayer->call_query(pt) : NULL;
    if (queryObject)
        return;
    queryEntity = engine.current_world ? currscn->call_query(pt) : NULL;
}

bool sreGUI::Object::is_hovering() const
{
    return queryObject == this;
}

// _query(float*) functions for both UI and World bases