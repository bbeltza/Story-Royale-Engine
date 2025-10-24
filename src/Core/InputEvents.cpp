#include <standard>

#include "../internal.h"

#include "Base/Input.hpp"

#include "Datatypes/Flags.hpp"

Signal<const Key*> Input::KeyEvent{NULL, false};
Signal<const TouchFinger*> Input::FingerMove{ NULL, false };
Signal<const MouseMove*> Input::MouseMove{NULL, false};
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

void __poll_input()
{
#define event engine.sdl_eventhndl
#define kbstate(s) reinterpret_cast<Flags8*>(&engine.keyboard_state[event.key.keysym.s])
    switch (event.type)
    {
    case SDL_MOUSEMOTION:
        mMoveSignalQueue.emplace_back(&event.motion, engine.real_scale);
        break;
    case SDL_MOUSEBUTTONDOWN:
        mButtonSignalQueue.emplace_back(&event.button, engine.real_scale);
        break;
    case SDL_MOUSEBUTTONUP:
        mButtonSignalQueue.emplace_back(&event.button, engine.real_scale);
        break;
    case SDL_MOUSEWHEEL:
        mWheelSignalQueue.emplace_back(&event.wheel, engine.real_scale);
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym > SDL_NUM_SCANCODES)
            kbstate(scancode)->ToggleOn(3);
        else
        {
            kbstate(scancode)->ToggleOn(1);
            kbstate(sym)->ToggleOn(2);
        }

        keySignalQueue.emplace_back(&event.key);
        break;
    case SDL_KEYUP:
        if (event.key.keysym.sym > SDL_NUM_SCANCODES)
            kbstate(scancode)->ToggleOff(3);
        else
        {
            kbstate(scancode)->ToggleOff(1);
            kbstate(sym)->ToggleOff(2);
        }
        keySignalQueue.emplace_back(&event.key);
        break;
    case SDL_FINGERDOWN:
        engine.input_last_touchid = event.tfinger.touchId;
        tFingerSignalQueue.emplace_back(&event.tfinger, true);
        break;
    case SDL_FINGERUP:
        tFingerSignalQueue.emplace_back(&event.tfinger, false);
        break;
    case SDL_FINGERMOTION:
        tMotionSignalQueue.emplace_back(&event.tfinger, true);
        break;
    default:
        break;
    }
}

void __update_input()
{
    if (!engine.viewport.w || !engine.viewport.h) return;

    int x, y;
    engine.mouse_press = SDL_GetMouseState(&x, &y);

    {
        register float w, h;
        w = static_cast<float>(engine.osize_x / engine.viewport.w);
        h = static_cast<float>(engine.osize_y / engine.viewport.h);
        engine.real_scale = fminf(w, h);
    }

    engine.mouse_x = x / engine.real_scale;
    engine.mouse_y = y / engine.real_scale;

    processEventQueue(keySignalQueue, Input::KeyEvent)
    processEventQueue(mButtonSignalQueue, Input::MouseButton)
    processEventQueue(mMoveSignalQueue, Input::MouseMove)
    processEventQueue(mWheelSignalQueue, Input::MouseWheel)
    processEventQueue(tFingerSignalQueue, Input::FingerTouch)
    processEventQueue(tMotionSignalQueue, Input::FingerMove)
}