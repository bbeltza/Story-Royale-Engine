#include "../internal.h"

#include <Core/Input.hpp>
#include <Core/Display.hpp>

#include <ECS/scene.hpp>
#include <ECS/entity.hpp>

#include <GUI/object.hpp>

void __query_objects()
{
    if (!engine.current_guilayer && !engine.current_world) return;

    sre::vec2ut pt{engine.mouse_x, engine.mouse_y};

    if (engine.input_last_touchid < 0) goto callsection;
    {
        int finger_count = SDL_GetNumTouchFingers(engine.input_last_touchid);
        if (!finger_count) goto callsection;

        
        SDL_Finger* lastfinger = SDL_GetTouchFinger(engine.input_last_touchid, finger_count - 1);

        pt = sre::display_size() * sre::vec2ut{ lastfinger->x, lastfinger->y };
    }


    callsection:

    if (
        engine.current_guilayer && currlayer->call_query(pt) &&
        engine.current_world && currscn->call_query(pt)
            ) (void)0;
}

// _query(float*) functions for both UI and World bases