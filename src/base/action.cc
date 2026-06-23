#include <Core/Input.hpp>
#include <Base/Action.h>
#include <Base/Signal.h>
#include <Base/Error.h>

#include "../internal.h"

#define ACTION_ISNULL sre::error(SRE_ERR_INVALID_VALUE, "action", NULL)

bool sre_actionpressed(const sre_Action action)
{
    if (!action)
    {
        ACTION_ISNULL;
        return false;
    }

    const sre_ActionInput* current = action;
    while (current->type)
    {
        bool done;
        switch (current->type)
        {
            case SRE_ACTION_MOUSE: done = sre::button_pressed(static_cast<sre::mouseButton>(current->code)); break;
            case SRE_ACTION_TOUCH: done = sre::fingers_pressed() != 0; break;
            case SRE_ACTION_KEYBOARD: done = sre::key_pressed(static_cast<sre::scanCode>(current->code)); break;
            case SRE_ACTION_VKEYBOARD: done = sre::key_pressed(static_cast<sre::keyCode>(current->code)); break;
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
    if (!action)
    {
        ACTION_ISNULL;
        return false;
    }

    const sre_ActionInput* current = action;
    while (current->type)
    {
        bool done;
        switch (current->type)
        {
            case SRE_ACTION_MOUSE: done = sre::button_justpressed(static_cast<sre::mouseButton>(current->code)); break;
            case SRE_ACTION_TOUCH: done = engine.lastfingerup; break;
            case SRE_ACTION_KEYBOARD: done = sre::key_justpressed(static_cast<sre::scanCode>(current->code)); break;
            case SRE_ACTION_VKEYBOARD: done = sre::key_justpressed(static_cast<sre::keyCode>(current->code)); break;
            default: abort();
        }

        if (done)
            return true;
        
        current++;
    }

    return false;
}