#include <standard>

#include "../internal.h"
#include "drivers/drivers.h"

#include "Base/Input.hpp"
#include "Base/Display.hpp"

#include "Datatypes/Flags.hpp"

Signal<Key> Input::KeyEvent{ NULL };
Signal<TouchFinger> Input::FingerMove{ NULL };
Signal<MouseMove> Input::MouseMove{ NULL };
Signal<MouseButton> Input::MouseButton;
Signal<MouseWheel> Input::MouseWheel;
Signal<TouchFinger> Input::FingerTouch;

void __poll_input(SDL_Event* event)
{
#define kbstate(s) flags_kbstate[event->key.keysym.s]
    switch (event->type)
    {
    case SDL_MOUSEMOTION:
        Input::MouseMove.Fire({ &event->motion, engine.video->scale });
        break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        Input::MouseButton.Fire({ &event->button, engine.video->scale });
        break;
    case SDL_MOUSEWHEEL:
        Input::MouseWheel.Fire({ &event->wheel, engine.video->scale });
        break;
    case SDL_KEYDOWN:
        if (event->key.keysym.sym > SDL_NUM_SCANCODES)
            kbstate(scancode).toggle_on(3);
        else
        {
            kbstate(scancode).toggle_on(1);
            kbstate(sym).toggle_on(2);
        }

        Input::KeyEvent.Fire({&event->key});
        break;
    case SDL_KEYUP:
        if (event->key.keysym.sym > SDL_NUM_SCANCODES)
            kbstate(scancode).toggle_off(3);
        else
        {
            kbstate(scancode).toggle_off(1);
            kbstate(sym).toggle_off(2);
        }
        Input::KeyEvent.Fire({&event->key});
        break;
    case SDL_FINGERDOWN:
        engine.input_last_touchid = event->tfinger.touchId;
        Input::FingerTouch.Fire({ &event->tfinger, true });
        break;
    case SDL_FINGERUP:
        Input::FingerTouch.Fire({ &event->tfinger, false });
        break;
    case SDL_FINGERMOTION:
        Input::FingerMove.Fire({ &event->tfinger, true });
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

    engine.mouse_x = x / engine.video->scale;
    engine.mouse_y = y / engine.video->scale;
}