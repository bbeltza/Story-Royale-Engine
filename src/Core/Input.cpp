#include "Base/Input.hpp"
#include "Game/World.hpp"


// Input functions

bool InputClass::isKeyPressed(SDL_KeyCode keycode) const
{
    return m_keyboardState[keycode & ~(1 << 30)].Has(2);
}

bool InputClass::isKeyPressed(SDL_Scancode scancode) const
{
    return m_keyboardState[scancode].Has(1);
}

bool InputClass::isMouseButtonPressed(MouseButton button) const
{
    if (button == mbANY) return m_mouseState.state.Has(mbANY);
    return m_mouseState.state.Has(SDL_BUTTON(button));
}

int InputClass::getFingersPressed(SDL_TouchID touchid) const
{
    if (mlast_touchid < 0) return false;
    if (touchid < 0) touchid = mlast_touchid;

    return SDL_GetNumTouchFingers(touchid);
}

const Vector2f InputClass::getMouseWorldPosition() const
{
    if (!Game::World::m_Current)
        return Game::World::screenToWorld(m_mouseState.x, m_mouseState.y);
    return Game::World::m_Current->screenToWorldSpace(m_mouseState.x, m_mouseState.y);
}