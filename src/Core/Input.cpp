#include "Input.h"
#include "ECS.h"


// Input functions

bool InputClass::isKeyPressed(SDL_KeyCode keycode) const
{
    return m_keyboardState[keycode & ~(1 << 30)] & 2;
}

bool InputClass::isKeyPressed(SDL_Scancode scancode) const
{
    return m_keyboardState[scancode] & 1;
}

bool InputClass::isMouseButtonPressed(ENUM_MouseButton button) const
{
    if (button == mbANY) return m_mouseState.state & mbANY;
    return m_mouseState.state & SDL_BUTTON(button);
}

const Vector2i InputClass::getMouseScreenPosition() const
{
    return Vector2i(m_mouseState.x, m_mouseState.y);
}

const Vector2f InputClass::getMouseWorldPosition() const
{
    return Game::currentWorld->screenToWorldSpace(m_mouseState.x, m_mouseState.y);
}