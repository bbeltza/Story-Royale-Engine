#include <standard.h>
#include "Engine.h"

#define processEventQueue(queue, ev) while (!queue.empty()) { ev.Fire(&queue.back()); queue.pop_back(); }

static std::vector<Key> keySignalQueue;
static std::vector<MouseButton> mButtonSignalQueue;
static std::vector<MouseMove> mMoveSignalQueue;
static std::vector<MouseWheel> mWheelSignalQueue;

void InputClass::processEvents()
{
    int x, y;
    m_mouseState.state = SDL_GetMouseState(&x, &y);
    
    Vector2i ratio = m_Engine->DrawingContext.getScreenAbsoluteSize() / m_Engine->DrawingContext.getScreenSize();
    m_mouseState.x = (float)x / ratio.X;
    m_mouseState.y = (float)y / ratio.Y;

    processEventQueue(keySignalQueue, keyEvent)
    processEventQueue(mButtonSignalQueue, mouseButton)
    processEventQueue(mMoveSignalQueue, mouseMove)
    processEventQueue(mWheelSignalQueue, mouseWheel)
}

void InputClass::processWindowEvents(SDL_Event* event)
{
    switch (event->type)
    {
    case SDL_MOUSEMOTION:
        mMoveSignalQueue.emplace_back(&event->motion);
        break;
    case SDL_MOUSEBUTTONDOWN:
        mButtonSignalQueue.emplace_back(&event->button);
        break;
    case SDL_MOUSEBUTTONUP:
        mButtonSignalQueue.emplace_back(&event->button);
        break;
    case SDL_MOUSEWHEEL:
        mWheelSignalQueue.emplace_back(&event->wheel);
        break;
    case SDL_KEYDOWN:
        if (event->key.keysym.sym > SDL_NUM_SCANCODES)
            m_keyboardState[event->key.keysym.scancode] |= 3;
        else
        {
            m_keyboardState[event->key.keysym.scancode] |= 1;
            m_keyboardState[event->key.keysym.sym] |= 2;
        }

        keySignalQueue.emplace_back(&event->key);
        break;
    case SDL_KEYUP:
        if (event->key.keysym.sym > 512)
            m_keyboardState[event->key.keysym.scancode] &= ~3;
        else
        {
            m_keyboardState[event->key.keysym.scancode] &= ~1;
            m_keyboardState[event->key.keysym.sym] &= ~2;
        }
        keySignalQueue.emplace_back(&event->key);
        break;
    default:
        break;
    }
}