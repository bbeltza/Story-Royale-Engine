#ifndef _WIN32
    #error "Can only compile for Windows"
#endif
#include <utils/logging.h>
#include <Windows.h>


typedef LPVOID coroutine_native;

static bool sys_coroutinecreate(coroutine_native* coroutine)
{
    LOG("Called this: sys_coroutinecreate() Ready to make a fiber!");
    return false;
}