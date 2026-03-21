#include <ucontext.h>

#include <Base/Log.h>

typedef ucontext_t coroutine_native;

static void coroutine_entry(void* data);


#define UC_STACKSIZE (8 * 1024 * 1024) // Hmm maybe using a constant for the stack size isn't the best idea
                                        // I just don't know a less platform-speficic way to determine the current stack size

static bool sys_coroutinecreate(coroutine_native* coroutine, const coroutine_data* data)
{
    if (getcontext(coroutine) < 0) return false;

    coroutine->uc_stack.ss_size = UC_STACKSIZE;
    coroutine->uc_stack.ss_sp = sre_new(UC_STACKSIZE);
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