#include <standard>

#include "../internal.h"
#include "drivers/drivers.h"

#include "Base/Input.hpp"
#include "Base/Display.hpp"

#include "Datatypes/Flags.hpp"

Signal<const Key*> Input::KeyEvent{ NULL };
Signal<const TouchFinger*> Input::FingerMove{ NULL };
Signal<const MouseMove*> Input::MouseMove{ NULL };
Signal<const MouseButton*> Input::MouseButton;
Signal<const MouseWheel*> Input::MouseWheel;
Signal<const TouchFinger*> Input::FingerTouch;

#define processEventQueue(queue, ev) while (!queue.empty()) { ev.Fire(&queue.back()); queue.pop_back(); }

static std::vector<Key> keySignalQueue;
static std::vector<TouchFinger> tFingerSignalQueue;
static std::vector<TouchFinger> tMotionSignalQueue;
static std::vector<MouseButton> mButtonSignalQueue;
static std::vector<MouseMove> mMoveSignalQueue;
static std::vector<MouseWheel> mWheelSignalQueue;

void __poll_input(SDL_Event* event)
{
#define kbstate(s) flags_kbstate[event->key.keysym.s]
    switch (event->type)
    {
    case SDL_MOUSEMOTION:
        mMoveSignalQueue.emplace_back(&event->motion, engine.video->scale);
        break;
    case SDL_MOUSEBUTTONDOWN:
        mButtonSignalQueue.emplace_back(&event->button, engine.video->scale);
        break;
    case SDL_MOUSEBUTTONUP:
        mButtonSignalQueue.emplace_back(&event->button, engine.video->scale);
        break;
    case SDL_MOUSEWHEEL:
        mWheelSignalQueue.emplace_back(&event->wheel, engine.video->scale);
        break;
    case SDL_KEYDOWN:
        if (event->key.keysym.sym > SDL_NUM_SCANCODES)
            kbstate(scancode).toggle_on(3);
        else
        {
            kbstate(scancode).toggle_on(1);
            kbstate(sym).toggle_on(2);
        }

        keySignalQueue.emplace_back(&event->key);
        break;
    case SDL_KEYUP:
        if (event->key.keysym.sym > SDL_NUM_SCANCODES)
            kbstate(scancode).toggle_off(3);
        else
        {
            kbstate(scancode).toggle_off(1);
            kbstate(sym).toggle_off(2);
        }
        keySignalQueue.emplace_back(&event->key);
        break;
    case SDL_FINGERDOWN:
        engine.input_last_touchid = event->tfinger.touchId;
        tFingerSignalQueue.emplace_back(&event->tfinger, true);
        break;
    case SDL_FINGERUP:
        tFingerSignalQueue.emplace_back(&event->tfinger, false);
        break;
    case SDL_FINGERMOTION:
        tMotionSignalQueue.emplace_back(&event->tfinger, true);
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

    processEventQueue(keySignalQueue, Input::KeyEvent)
    processEventQueue(mButtonSignalQueue, Input::MouseButton)
    processEventQueue(mMoveSignalQueue, Input::MouseMove)
    processEventQueue(mWheelSignalQueue, Input::MouseWheel)
    processEventQueue(tFingerSignalQueue, Input::FingerTouch)
    processEventQueue(tMotionSignalQueue, Input::FingerMove)
}