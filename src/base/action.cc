#include <Core/Input.hpp>
#include <Base/Action.h>
#include <Base/Signal.h>

bool sre_actionpressed(const sre_Action action)
{
    const sre_ActionInput* current = action;
    while (current->type)
    {
        bool done;
        switch (current->type)
        {
            case sre::ACTION_MOUSE: done = sre::button_pressed(static_cast<sre::mouseButton>(current->code)); break;
            case sre::ACTION_TOUCH: done = false; break;
            case sre::ACTION_KEYBOARD: done = sre::key_pressed(static_cast<sre::scanCode>(current->code)); break;
            case sre::ACTION_VKEYBOARD: done = sre::key_pressed(static_cast<sre::keyCode>(current->code)); break;
            default: abort();
        }

        if (done)
            return true;
        
        current++;
    }

    return false;
}

bool sre_actionjustpressed(const sre_Action action)
{
    const sre_ActionInput* current = action;
    while (current->type)
    {
        bool done;
        switch (current->type)
        {
            case sre::ACTION_MOUSE: done = sre::button_justpressed(static_cast<sre::mouseButton>(current->code)); break;
            case sre::ACTION_TOUCH: done = false; break;
            case sre::ACTION_KEYBOARD: done = sre::key_justpressed(static_cast<sre::scanCode>(current->code)); break;
            case sre::ACTION_VKEYBOARD: done = sre::key_justpressed(static_cast<sre::keyCode>(current->code)); break;
            default: abort();
        }

        if (done)
            return true;
        
        current++;
    }

    return false;
}