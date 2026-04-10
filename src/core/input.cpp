#include "../internal.h"

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
    case SDL_MOUSEMOTION:
        engine.mouse_x = event->motion.x * engine.scale_ratio;
        engine.mouse_y = event->motion.y * engine.scale_ratio;
        break;
    case SDL_MOUSEBUTTONUP:
        engine.mouse_press &= ~SDL_BUTTON(event->button.button);
        break;
    case SDL_MOUSEBUTTONDOWN:
        engine.mouse_press |= SDL_BUTTON(event->button.button);
        break;
    default:
        break;
    }
}

//

bool sre::key_pressed(sre::keyCode code) { return key_pressed(static_cast<sre::scanCode>(SDL_GetScancodeFromKey(code))); }
bool sre::key_pressed(sre::scanCode code) { return 0 != (engine.keyboard_state[code / 8] & (1 << (code % 8))); }

bool sre::button_pressed(sre::mouseButton button) { return 0 != (engine.mouse_press & SDL_BUTTON(button)); }

sre::vec2ut sre::mouse_screencoords() { return sre::vec2ut{ engine.mouse_x, engine.mouse_y }; }

int sre::fingers_pressed()
{
    if (engine.input_last_touchid < 0) return 0;
    return SDL_GetNumTouchFingers(engine.input_last_touchid);
}

bool sre::key_justpressed(sre::keyCode code) { return key_pressed(static_cast<sre::scanCode>(SDL_GetScancodeFromKey(code))); }
bool sre::key_justpressed(sre::scanCode code) { return 0 != (engine.keyboard_framestate[code / 8] & (1 << (code % 8))); }
bool sre::button_justpressed(sre::mouseButton button) { return 0 != (engine.mouse_framepress & SDL_BUTTON(button)); }