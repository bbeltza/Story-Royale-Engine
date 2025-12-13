#include "../internal.h"

#include "Base/Input.hpp"
#include "Game/World.hpp"

#include "datatypes/Flags.hpp"

bool Input::KeyPressed(SDL_KeyCode keycode)
{
    return flags_kbstate[keycode & ~(1 << 30)].Has(2);
}

bool Input::KeyPressed(SDL_Scancode scancode)
{
    return flags_kbstate[scancode].Has(1);
}

bool Input::MouseButtonPressed(Button button)
{
    if (button == mbANY)
        return flags_mousepress.Has(mbANY);
    return flags_mousepress.Has(SDL_BUTTON(button));
}

int Input::GetFingersPressed(SDL_TouchID touchid)
{
    if (engine.input_last_touchid < 0) return false;
    if (touchid < 0) touchid = engine.input_last_touchid;

    return SDL_GetNumTouchFingers(touchid);
}

sre::vec2ut Input::MouseScreenPosition()
{
    return { engine.mouse_x, engine.mouse_y };
}

sre::vec2ut Input::MouseWorldPosition()
{
    return Game::World::screenToWorld(engine.mouse_x, engine.mouse_y, Game::World::currentCamera());
}