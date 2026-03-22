#ifndef SRE_ACTION_H
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

typedef struct sre_Action
{
    struct sre_ActionInput const *inputs;

    #ifdef __cplusplus
        template <size_t n>
        sre_Action(const sre_ActionInput (&inputs)[n]): inputs(inputs) {}
            
        inline bool pressed() const;
        inline bool just_pressed() const;
    #endif
} sre_Action;

#ifdef __cplusplus
}
#endif

bool sre_actionpressed(const sre_Action* action);
bool sre_actionjustpressed(const sre_Action* action);

struct sre_Signal;
// Make a signal that fires when one of the inputs in `action` get pressed
struct sre_Signal* sre_actionsignal(const sre_Action* action);

#ifndef __cplusplus
    #define SRE_MAKE_ACTION(...) { (struct sre_ActionInput[]){__VA_ARGS__, {SRE_ACTION_NULL, 0}} }
#else
    extern "C++" {
        bool sre_Action::pressed() const { return sre_actionpressed(this); }
        bool sre_Action::just_pressed() const { return sre_actionjustpressed(this); }
        
        struct sre_Connection;
        namespace sre
        {
            using Action = sre_Action;

            extern sre_Connection* action_signal(const Action& action);
        }
    }
    

    #define SRE_MAKE_ACTION(...) ::sre::Action{ { __VA_ARGS__, {::sre::ACTION_NULL, 0} } }
#endif

SRE_CAPI_END

#endif