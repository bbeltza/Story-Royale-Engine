#include <standard>
#include "Engine.hpp"
#include "Events/Key.hpp"
#include "Events/Mouse.hpp"
#include "Events/Touch.hpp"

#define processEventQueue(queue, ev) while (!queue.empty()) { ev.Fire(&queue.back()); queue.pop_back(); }

static std::vector<Key> keySignalQueue;
static std::vector<TouchFinger> tFingerSignalQueue;
static std::vector<TouchFinger> tMotionSignalQueue;
static std::vector<MouseButton> mButtonSignalQueue;
static std::vector<MouseMove> mMoveSignalQueue;
static std::vector<MouseWheel> mWheelSignalQueue;

void InputClass::processEvents()
{
    int x, y;
    m_mouseState.state = SDL_GetMouseState(&x, &y);
    
    m_scaleratio = (float)(m_Engine->DrawingContext.getScreenAbsoluteSize() / m_Engine->DrawingContext.getScreenSize()).getMax();
    m_mouseState.x = x / m_scaleratio;
    m_mouseState.y = y / m_scaleratio;

    processEventQueue(keySignalQueue, keyEvent)
    processEventQueue(mButtonSignalQueue, mouseButton)
    processEventQueue(mMoveSignalQueue, mouseMove)
    processEventQueue(mWheelSignalQueue, mouseWheel)
    processEventQueue(tFingerSignalQueue, fingerTouch)
    processEventQueue(tMotionSignalQueue, fingerMove)
}

void InputClass::processWindowEvents(SDL_Event* event)
{
    switch (event->type)
    {
    case SDL_MOUSEMOTION:
        mMoveSignalQueue.emplace_back(&event->motion, m_scaleratio);
        break;
    case SDL_MOUSEBUTTONDOWN:
        mButtonSignalQueue.emplace_back(&event->button, m_scaleratio);
        break;
    case SDL_MOUSEBUTTONUP:
        mButtonSignalQueue.emplace_back(&event->button, m_scaleratio);
        break;
    case SDL_MOUSEWHEEL:
        mWheelSignalQueue.emplace_back(&event->wheel, m_scaleratio);
        break;
    case SDL_KEYDOWN:
        if (event->key.keysym.sym > SDL_NUM_SCANCODES)
            m_keyboardState[event->key.keysym.scancode].ToggleOn(3);
        else
        {
            m_keyboardState[event->key.keysym.scancode].ToggleOn(1);
            m_keyboardState[event->key.keysym.sym].ToggleOn(2);
        }

        keySignalQueue.emplace_back(&event->key);
        break;
    case SDL_KEYUP:
        if (event->key.keysym.sym > 512)
            m_keyboardState[event->key.keysym.scancode].ToggleOff(3);
        else
        {
            m_keyboardState[event->key.keysym.scancode].ToggleOff(1);
            m_keyboardState[event->key.keysym.sym].ToggleOff(2);
        }
        keySignalQueue.emplace_back(&event->key);
        break;
    case SDL_FINGERDOWN:
        mlast_touchid = event->tfinger.touchId;
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