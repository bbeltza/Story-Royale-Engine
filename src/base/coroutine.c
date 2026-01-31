#include <Base/Coroutine.h>
#include <utils/mem.h>

#include <SDL_thread.h>
#include <SDL_timer.h>

/* A short explanation about coroutine's implementation... */
/* There's a folder in src/base called `coroutine`, it contains every single needed 
   OS/architecture implementation for coroutines to work */
/* Depending on the OS, or perhaps even the architecture, this file will include the corresponding
   source file in src/base/coroutine to define the correct functions */
/* They are included using #include, they are NOT added in cmake, making this a little more build-system
   independent while still being flexible */

/* The following implementation files in src/base/coroutine should contain the following symbols: */
    // A typedef to a `coroutine_native`, it's the struct containing platform-speficic data
    // An #include to a header called "internal.h" (Maybe not for now). It contains common internal declarations
    // A set of `sys` functions below, all commented

/* There will also be two functions used by the engine that are not included in `Coroutine.h` to initialize and deinitialize the coroutine engine */
    // sre_coroutinecoreinit()
    // sre_coroutinecorequit()

#if defined(_WIN32)
    #include "coroutine/win32.c"
#else
    #error "Make an implementation!! Lazy..."
#endif

/* `sys_` functions */
// Setup to the coroutine (using CreateFiber on win32 for example)
static bool sys_coroutinecreate(coroutine_native* coroutine, sre_coroutineFunction func, void* userdata);
// Setup the thread pool context coroutine (On win32 it will just call SwitchThreadToFiber once if what's in `pool` is NULL)
// `pool` is automatically initialized to `NULL` for the first time
static bool sys_coroutinepoolsetup(coroutine_native* pool);
// Switch to the following coroutine
static void sys_coroutineswitch(const coroutine_native* coroutine);

struct sre_coroutine
{
    sre_coroutine* next;
    sre_coroutineState state;

    coroutine_native native;
};

// Data containing the necessary stuff to run the coroutine
struct coroutine_instance
{
    SDL_Thread* thread;
    SDL_threadID thread_id;

    sre_coroutine* current;
    sre_coroutine* head;

    coroutine_native thread_native;
};

static struct coroutine_instance inst;

static int SDLCALL poolthread_proc(void* _inst)
{
    struct coroutine_instance* inst = _inst;
    inst->thread_id = SDL_ThreadID();

    while (true)
    {
        sre_coroutine* current = inst->head;
        while (current)
        {
            assert(current->state != SRE_COROUTINESTATE_INVALID);
            if (current->state == SRE_COROUTINESTATE_SUSPENDED) goto ENDLOOP;

            // Code to perform the OS switch
                sys_coroutinepoolsetup(&inst->thread_native);
                sys_coroutineswitch(&current->native);
            //

            ENDLOOP:
            current = current->next;
        }

        SDL_Delay(1);
    }

    return 0;
}

bool sre_coroutinecoreinit()
{
    assert(inst.thread == NULL /* Cannot call sre_coroutinecoreinit() twice!! */);
    
    inst.thread = SDL_CreateThread(poolthread_proc, "Coroutine pool", &inst);
    if (!inst.thread)
        return false;
    
    return true;
}

void sre_coroutinecorequit()
{
    SDL_DetachThread(inst.thread);
}

sre_coroutine* sre_coroutinecreate(bool suspended, sre_coroutineFunction function, void* userdata)
{
    sre_coroutine* coroutine = sre_new(sizeof(sre_coroutine));
    memset(coroutine, 0, sizeof(coroutine));

    if (!sys_coroutinecreate(&coroutine->native, function, userdata))
    {
        sre_delete(coroutine);
        return NULL;
    }
    
    coroutine->state = suspended ? SRE_COROUTINESTATE_SUSPENDED : SRE_COROUTINESTATE_RUNNING;
    coroutine->next = inst.head;
    inst.head = coroutine;

    return coroutine;
}

bool sre_coroutineresume(sre_coroutine* coroutine)
{
    if (!coroutine) return false;
    coroutine->state = SRE_COROUTINESTATE_RUNNING;
    return true;
}

bool sre_coroutinesuspend()
{
    if (SDL_ThreadID() != inst.thread_id) return false;

    assert(inst.current != NULL);
    inst.current->state = SRE_COROUTINESTATE_SUSPENDED;
    
    sys_coroutineswitch(&inst.thread_native);

    return true;
}

bool sre_coroutineyield(sre_timeStamp time)
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