#include <Base/Coroutine.h>
#include <utils/mem.h>

/* A short explanation about coroutine's implementation... */
/* There's a folder in src/base called `coroutine`, it contains every single needed 
   OS/architecture implementation for coroutines to work */
/* Depending on the OS, or perhaps even the architecture, this file will include the corresponding
   source file in src/base/coroutine to define the correct functions */
/* They are included using #include, they are NOT added in cmake, making this a little more build-system
   independent while still being flexible */

/* The following implementation files in src/base/coroutine should contain the following symbols: */
    // A typedef to a `coroutine_native`, it's the struct containing platform-speficic data
    // An #include to a header called "internal.h". It contains common internal declarations
    // bool sys_coroutinecreate(coroutine_native* coroutine) -> Setup to the coroutine (using CreateFiber on win32 for example)
    // void sys_coroutineswitch(coroutine_native* coroutine) -> Switch to the following coroutine
                                                        //     -> It doesn't return, if it returns then you could assume there's an error

#if defined(_WIN32)
    #include "coroutine/win32.c"
#else
    #error "Make an implementation!! Lazy..."
#endif

static bool sys_coroutinecreate(coroutine_native* coroutine);
static void sys_coroutineswitch(coroutine_native* coroutine);

struct sre_coroutine
{
    sre_coroutine* next;
    sre_coroutineState state;

    coroutine_native native;
};

sre_coroutine* sre_coroutinecreate(bool suspended, sre_coroutineFunction function, void* userdata)
{
    sre_coroutine* coroutine = sre_new(sizeof(sre_coroutine));
    memset(coroutine, 0, sizeof(coroutine));

    if (!sys_coroutinecreate(&coroutine->native))
    {
        sre_delete(coroutine);
        return NULL;
    }
    
    coroutine->state = suspended ? SRE_COROUTINESTATE_SUSPENDED : SRE_COROUTINESTATE_RUNNING;

    return coroutine;
}

bool sre_coroutineresume(sre_coroutine* coroutine)
{
    return false;
}
bool sre_coroutinesuspend(sre_coroutine* coroutine)
{
    return false;
}
bool sre_coroutineyield(sre_coroutine* coroutine, sre_timeStamp time)
{
    return false;
}

sre_coroutineState sre_coroutinestate(const sre_coroutine* coroutine)
{
    return SRE_COROUTINESTATE_INVALID;
}

void sre_coroutinecancel(sre_coroutine* coroutine)
{

}

void sre_coroutineclose(sre_coroutine* coroutine)
{

}