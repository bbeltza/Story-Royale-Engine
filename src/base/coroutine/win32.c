#ifndef _WIN32
    #error "Can only compile for Windows!!"
#endif
#include <utils/logging.h>
#include <Windows.h>


typedef LPVOID coroutine_native;

static bool sys_coroutinecreate(coroutine_native* coroutine, sre_coroutineFunction func, void* userdata) // Hmm... Intellisense doesn't seem to recognize the types defined in coroutine.c...
{
    LOG("Called this: sys_coroutinecreate() Ready to make a fiber!");

    LPVOID fiber = CreateFiber(0, (LPFIBER_START_ROUTINE)func, userdata);
    if (!fiber)
        return false;
    
    return true;
}