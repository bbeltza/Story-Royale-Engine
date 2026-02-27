#ifndef _WIN32
    #error "Can only compile for Windows!!"
#endif

#include <Windows.h>

#define COROUTINE_CALL WINAPI
typedef LPVOID coroutine_native;
static VOID COROUTINE_CALL coroutine_entry(LPVOID lpFiberParameter); // lpStartAddress is __stdcall while sre_coroutineFunction isn't. Use wrapper then...

static bool sys_coroutinecreate(coroutine_native* coroutine, const coroutine_data* data) // Hmm... Intellisense doesn't seem to recognize the types defined in coroutine.c...
{
    LPVOID fiber = CreateFiber(0, coroutine_entry, (LPVOID)data);
    if (!fiber)
        return false;
    
    *coroutine = fiber;
    return true;
}

static bool sys_coroutinepoolsetup(coroutine_native* pool)
{
    assert(pool != NULL);
    assert(*pool == NULL);

    *pool = ConvertThreadToFiber(NULL); // Uhh I don't know what to do with lpParameter it's probably not any important
    if (!*pool) return false;

    return true;
}

static void sys_coroutineswitch(coroutine_native* coroutine)
{
    assert(coroutine != NULL);

    LPVOID fiber = *coroutine;
    assert(fiber != NULL);

    SwitchToFiber(fiber);
}

static void sys_coroutineclose(coroutine_native* coroutine)
{
    assert(coroutine != NULL);

    DeleteFiber(*coroutine);
}