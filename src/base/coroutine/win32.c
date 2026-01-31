#ifndef _WIN32
    #error "Can only compile for Windows!!"
#endif
#include <utils/logging.h>
#include <Windows.h>

typedef LPVOID coroutine_native;

struct FIBER_PARAMETER
{
    sre_coroutineFunction func;
    void* data;
};

static VOID WINAPI fiber_entry(LPVOID lpFiberParameter) // lpStartAddress is __stdcall while sre_coroutineFunction isn't. Use wrapper then...
{
    ((struct FIBER_PARAMETER*)lpFiberParameter)->func(((struct FIBER_PARAMETER*)lpFiberParameter)->data);
    HeapFree(GetProcessHeap(), 0, lpFiberParameter);
}

static bool sys_coroutinecreate(coroutine_native* coroutine, sre_coroutineFunction func, void* userdata) // Hmm... Intellisense doesn't seem to recognize the types defined in coroutine.c...
{
    LOG("Called this: sys_coroutinecreate() Ready to make a fiber!");

    struct FIBER_PARAMETER* params = HeapAlloc(GetProcessHeap(), 0, sizeof(struct FIBER_PARAMETER));
    if (!params)
        return false;

    params->func = func;
    params->data = userdata;
    LPVOID fiber = CreateFiber(0, fiber_entry, params);
    if (!fiber)
        return false;
    
    *coroutine = fiber;
    return true;
}

static bool sys_coroutinepoolsetup(coroutine_native* pool)
{
    assert(pool != NULL);

    LPVOID fiber = *pool;
    if (fiber)
        return true;

    fiber = ConvertThreadToFiber(NULL); // Uhh I don't know what to do with lpParameter it's probably not any important
    if (!fiber) return false;

    *pool = fiber;
    return true;
}

static void sys_coroutineswitch(const coroutine_native* coroutine)
{
    assert(coroutine != NULL);

    LPVOID fiber = *coroutine;
    if (fiber == NULL) return;

    LPVOID thrd = ConvertThreadToFiber(NULL);
    SwitchToFiber(fiber);
}