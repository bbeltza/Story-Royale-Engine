#include "../internal.h"
#include "drivers/drivers.h"

#include <Base/Event.hpp>

Signal<sre::Event> sre::onEvent;

int __signal_events(void* data, SDL_Event* ev)
{
    sre::Event current;

    switch (ev->type)
    {
    case SDL_KEYUP:
    case SDL_KEYDOWN:
        current.type = sre::EVENT_KEYPRESS;
        current.key_press.scancode = static_cast<sre::scanCode>(ev->key.keysym.scancode);
        current.key_press.keycode = static_cast<sre::keyCode>(ev->key.keysym.sym);
        current.key_press.keymod = static_cast<sre::keyMod>(ev->key.keysym.mod);

        current.key_press.press = ev->key.state + ev->key.repeat;
        break;
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
        current.type = sre::EVENT_MOUSEBUTTON;
        current.mouse_button.id = ev->button.which;
        current.mouse_button.button = ev->button.button;
        current.mouse_button.clicks = ev->button.clicks;
        current.mouse_button.pressed = ev->button.state;
        current.mouse_button.position = sre::vec2ut{ ev->button.x, ev->button.y } * engine.scale_ratio;
        break;
    case SDL_MOUSEMOTION:
        current.type = sre::EVENT_MOUSEMOVE;
        current.mouse_move.id = ev->motion.which;
        current.mouse_move.button_state = ev->motion.state;
        current.mouse_move.position = sre::vec2ut{ ev->motion.x, ev->motion.y } * engine.scale_ratio;
        current.mouse_move.delta = sre::vec2ut{ ev->motion.xrel, ev->motion.yrel } * engine.scale_ratio;
        break;
    case SDL_FINGERUP:
    case SDL_FINGERDOWN:
    case SDL_FINGERMOTION:
        current.type = sre::EVENT_TOUCH;
        current.touch.uv = sre::vec2ut{ ev->tfinger.x, ev->tfinger.y };
        current.touch.delta = sre::vec2ut{ ev->tfinger.dx, ev->tfinger.dy };
        current.touch.pressure = ev->tfinger.pressure;
        current.touch.pressed = ev->type != SDL_FINGERUP;
        break;
    default:
        return 1;
    }
    sre::onEvent.Fire(current);

    return 1;
}