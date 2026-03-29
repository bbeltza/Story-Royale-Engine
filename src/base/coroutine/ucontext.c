#include <ucontext.h>

#include <Base/Log.h>

typedef ucontext_t coroutine_native;

static void coroutine_entry(void* data);

#define COROUTINE_STACKVAR uc_stack.s_sp
#define COROUTINE_STACKSIZE uc_stack.s_size

static bool sys_coroutinecreate(coroutine_native* coroutine, const coroutine_data* data)
{
    makecontext(coroutine, (void(*)(void))coroutine_entry, 1, data);
    return true;
}

static bool sys_coroutinepoolsetup(coroutine_native* pool)
{
    (void)pool;
    return true;
}

static void sys_coroutineswitch(coroutine_native* coroutine, coroutine_native* current)
{
    static coroutine_native _dummy;
    if (!current)
    {
        setcontext(coroutine);
        return;
    }

    swapcontext(current, coroutine);
}

static void sys_coroutineclose(coroutine_native* coroutine)
{
    sre_delete(coroutine->uc_stack.ss_sp);
}