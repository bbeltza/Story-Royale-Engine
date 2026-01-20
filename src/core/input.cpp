#include "../internal.h"
#include "drivers/drivers.h"

#include <Core/Input.hpp>

void __poll_input(SDL_Event* event)
{
    switch (event->type)
    {
    case SDL_KEYDOWN:
        engine.keyboard_state[event->key.keysym.scancode / 8] |= (1 << (event->key.keysym.scancode % 8));
        break;
    case SDL_KEYUP:
        engine.keyboard_state[event->key.keysym.scancode / 8] &= ~(1 << (event->key.keysym.scancode % 8));
        break;
    case SDL_FINGERDOWN:
        engine.input_last_touchid = event->tfinger.touchId;
        break;
    default:
        break;
    }
}

void __update_input()
{
    if (!engine.osize_x || !engine.osize_y) return;

    int x, y;
    engine.mouse_press = SDL_GetMouseState(&x, &y);

    engine.mouse_x = x * engine.scale_ratio;
    engine.mouse_y = y * engine.scale_ratio;
}

//

bool sre::key_pressed(sre::keyCode code) { return key_pressed(static_cast<sre::scanCode>(SDL_GetScancodeFromKey(code))); }
bool sre::key_pressed(sre::scanCode code) { return engine.keyboard_state[code / 8] & (1 << (code % 8)); }

bool sre::button_pressed(sre::mouseButton button) { return engine.mouse_press & SDL_BUTTON(button); }

sre::vec2ut sre::mouse_screencoords() { return sre::vec2ut{ engine.mouse_x, engine.mouse_y }; }

int sre::fingers_pressed()
{
    if (engine.input_last_touchid < 0) return 0;
    return SDL_GetNumTouchFingers(engine.input_last_touchid);
}