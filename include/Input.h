#pragma once
#include <SDL.hpp>

#include "Datatypes.h"
#include "EngineBase.h"

enum ENUM_MouseButton : Uint8
{
    mbLeft = SDL_BUTTON_LEFT,
    mbMiddle = SDL_BUTTON_MIDDLE,
    mbRight = SDL_BUTTON_RIGHT,
    mbX1 = SDL_BUTTON_X1,
    mbX2 = SDL_BUTTON_X2,


    mbANY = UINT8_MAX
};

class InputClass
{
    ENGINE_BASE
public:
    // Keyboard functions
    bool isKeyPressed(SDL_KeyCode keycode) const;
    bool isKeyPressed(SDL_Scancode scancode) const;

    // Mouse functions
    bool isMouseButtonPressed(ENUM_MouseButton button = mbANY) const;
    const Vector2i getMouseScreenPosition() const;
    const Vector2f getMouseWorldPosition() const;

    // Input events
    Signal keyEvent;
    Signal mouseButton;
    Signal mouseWheel;
    Signal mouseMove;

    //
    struct _mState { int x, y; uint32_t state; };
private:
    InputClass() {}

    void processWindowEvents(SDL_Event* event);
    void processEvents();
    void queryObjects();

    // Private states
    _mState m_mouseState;
    uint8_t m_keyboardState[SDL_NUM_SCANCODES];
};