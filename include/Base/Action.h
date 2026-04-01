#ifndef SRE_ACTION_H
#define SRE_ACTION_H
#include <C_API.h>

SRE_CAPI_BEGIN

#ifndef __cplusplus
    typedef enum sre_actionInputType
    {
        SRE_ACTION_NULL, // Should be the last input inside an action, made to indicate null-termination
                            // Don't bother using it, use the SRE_MAKE_ACTION() macro instead
        SRE_ACTION_MOUSE,
        SRE_ACTION_TOUCH,
        SRE_ACTION_KEYBOARD, // Keyboard type that uses scancodes
        SRE_ACTION_VKEYBOARD // Keyboard action type using key codes ("virtual keys")
    } sre_actionInputType;
#else
    namespace sre
    {
        enum actionInputType
        {
            ACTION_NULL,
            ACTION_MOUSE,
            ACTION_TOUCH,
            ACTION_KEYBOARD,
            ACTION_VKEYBOARD,
        };
    }

    using sre_actionInputType = sre::actionInputType;
#endif

struct sre_ActionInput
{
    sre_actionInputType type;
    int code;
};

#ifdef __cplusplus
extern "C++" {
#endif

typedef struct sre_ActionInput sre_Action[];

#ifdef __cplusplus
}
#endif

bool sre_actionpressed(const sre_Action action);
bool sre_actionjustpressed(const sre_Action action);

struct sre_Signal;
// Make a signal that fires when one of the inputs in `action` get pressed
struct sre_Signal* sre_actionsignal(const sre_Action* action);

#ifndef __cplusplus
    #define SRE_MAKE_ACTION(...) {__VA_ARGS__, {SRE_ACTION_NULL, 0}}
#else
    extern "C++" {

        struct sre_Connection;
        namespace sre
        {
            using Action = sre_Action;

            inline bool action_pressed(const Action act) { return sre_actionpressed(act); }
            inline bool action_justpressed(const Action act) { return sre_actionjustpressed(act); }

            extern sre_Connection* action_signal(const Action& action);
        }
    }
    

    #define SRE_MAKE_ACTION(...) { __VA_ARGS__, {::sre::ACTION_NULL, 0} }
#endif

SRE_CAPI_END

#endif