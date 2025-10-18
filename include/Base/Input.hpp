#pragma once
#include <SDL.hpp>

#include "EngineBase.hpp"

#include "Classes/Signal.hpp"

#include "Datatypes/Vector.hpp"
#include "Datatypes/Flags.hpp"

#include "Events/Key.hpp"
#include "Events/Mouse.hpp"
#include "Events/Touch.hpp"

class InputClass
{
    ENGINE_BASE
    InputClass(EngineClass* engine) : m_Engine(engine) { setupActions(); }
public:
    // Mouse button enum

    enum MouseButton : Uint8
{
    mbLeft = SDL_BUTTON_LEFT,
    mbMiddle = SDL_BUTTON_MIDDLE,
    mbRight = SDL_BUTTON_RIGHT,
    mbX1 = SDL_BUTTON_X1,
    mbX2 = SDL_BUTTON_X2,


    mbANY = UINT8_MAX
};

    // Keyboard functions
    bool isKeyPressed(SDL_KeyCode keycode) const;
    bool isKeyPressed(SDL_Scancode scancode) const;

    // Mouse functions
    bool isMouseButtonPressed(MouseButton button = mbANY) const;
    const Vector2f getMouseScreenPosition() const { return Vector2f(m_mouseState.x, m_mouseState.y); }
    const Vector2f getMouseWorldPosition() const;

    // Touch functions
    int getFingersPressed(SDL_TouchID touchid = -1) const;

    inline float getScaleRatio() const {return m_scaleratio;}

    // Input events

    Signal<const ::Key*> keyEvent{this, false};
    Signal<const ::MouseButton*> mouseButton{this, false};
    Signal<const ::MouseWheel*> mouseWheel{this, false};
    Signal<const ::MouseMove*> mouseMove{this, false};
    Signal<const ::TouchFinger*> fingerTouch{this, false};
    Signal<const ::TouchFinger*> fingerMove{this, false};

    // Future Input events
    /*
    Signal InputBegin{this};
    Signal InputEnd{this};
    Signal InputChange{this};
    */
    //
    struct _mState { float x, y; Flags32 state; };
private:
    void setupActions();
    void processWindowEvents(SDL_Event* event);
    void processEvents();
    void queryObjects();

    // Private states
    _mState m_mouseState;
    Flags8 m_keyboardState[SDL_NUM_SCANCODES] = {0};

    SDL_TouchID mlast_touchid = -1;

    float m_scaleratio;
};