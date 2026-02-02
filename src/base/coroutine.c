#include <Base/Coroutine.h>
#include <utils/mem.h>

#include <SDL_thread.h>
#include <SDL_timer.h>

#include <assert.h>

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
    // Perhaps a #define to the entry point's calling convention, it's optional

/* There will also be two functions used by the engine that are not included in `Coroutine.h` to initialize and deinitialize the coroutine engine */
    // sre_coroutinecoreinit()
    // sre_coroutinecorequit()
    
typedef struct coroutine_data coroutine_data;

#if defined(_WIN32)
    #include "coroutine/win32.c"
#elif __has_include("ucontext.h")
    #include "coroutine/ucontext.c"
#else
    #include "coroutine/dummy.c"
    #error "Make an implementation!! Lazy..."
#endif

#ifndef COROUTINE_CALL
    #define COROUTINE_CALL
#endif

/* `sys_` functions */
// Setup to the coroutine (using CreateFiber on win32 for example)
static bool sys_coroutinecreate(coroutine_native* coroutine, const coroutine_data* data);
// Setup the thread pool context coroutine (On win32 it will just call SwitchThreadToFiber once if what's in `pool` is NULL)
// `pool` is automatically initialized to `NULL` for the first time
static bool sys_coroutinepoolsetup(coroutine_native* pool);
// Switch to the following coroutine
static void sys_coroutineswitch(coroutine_native* coroutine);

static void sys_coroutineclose(coroutine_native* coroutine);

struct sre_coroutine
{
    sre_coroutine* next;
    sre_coroutineState state;

    void* data;

    coroutine_native native;
    Uint64 resume_tick;
};

// Data containing the necessary stuff to run the coroutine
struct coroutine_instance
{
    SDL_Thread* thread;
    SDL_threadID thread_id;

    sre_coroutine* current;
    sre_coroutine* head;
    sre_coroutine* end;

    coroutine_native thread_native;

    bool running;
};

struct coroutine_data
{
    sre_coroutineFunction func;
    void* userdata;

    sre_coroutineState* stateptr;
};

static struct coroutine_instance inst;

static int SDLCALL poolthread_proc(void* _inst)
{
    struct coroutine_instance* inst = _inst;
    inst->thread_id = SDL_ThreadID();
    inst->running = true;

    sys_coroutinepoolsetup(&inst->thread_native);

    while (inst->running || inst->current)
    {
        sre_coroutine* prev = NULL;
        inst->current = inst->head;
        while (inst->current)
        {
            assert(inst->current->state != SRE_COROUTINESTATE_INVALID);
            if (inst->current->state == SRE_COROUTINESTATE_SUSPENDED)
            {
                if (inst->current->resume_tick && inst->current->resume_tick <= SDL_GetTicks64())
                    inst->current->state = SRE_COROUTINESTATE_RUNNING;
                else
                    goto ENDLOOP;
            }

            // Code to perform the "OS" switch
                sys_coroutineswitch(&inst->current->native);
                if (inst->current->state == SRE_COROUTINESTATE_CANCELLED)
                {
                    sre_coroutine* curr = inst->current;
                    if (prev)
                        prev->next = curr->next;
                    else
                        inst->head = curr->next;

                    if (curr == inst->end)
                        inst->end = prev;
                    
                    inst->current = curr->next;

                    sys_coroutineclose(&curr->native);
                    sre_delete(curr);

                    continue;
                }
            //

            ENDLOOP:
            prev = inst->current;
            inst->current = inst->current->next;
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
    sre_coroutine* current = inst.head;
    while (current)
    {
        current->state = SRE_COROUTINESTATE_CANCELLED;
        current = current->next;
    }

    inst.running = false;
    SDL_WaitThread(inst.thread, NULL);
}

sre_coroutine* sre_coroutinecreate(bool suspended, sre_coroutineFunction function, void* userdata)
{
    sre_coroutine* coroutine = sre_newclear(sizeof(sre_coroutine));

    coroutine_data* data = sre_new(sizeof(coroutine_data));
    data->func = function;
    data->userdata = userdata;
    data->stateptr = &coroutine->state;

    if (!sys_coroutinecreate(&coroutine->native, data))
    {
        sre_delete(coroutine);
        return NULL;
    }
    
    coroutine->state = suspended ? SRE_COROUTINESTATE_SUSPENDED : SRE_COROUTINESTATE_RUNNING;
    if (inst.end)
    {
        inst.end->next = coroutine;
    }
    else
    {
        assert(inst.head == NULL);
        inst.head = coroutine;
    }
    inst.end = coroutine;

    return coroutine;
}

bool sre_coroutineresume(sre_coroutine* coroutine, void* data)
{
    if (!coroutine) return false;
    if (coroutine->state == SRE_COROUTINESTATE_SUSPENDED)
    {
        coroutine->data = data;
        coroutine->state = SRE_COROUTINESTATE_RUNNING;
    }
    return true;
}

bool sre_coroutinerunning()
{
    if (SDL_ThreadID() != inst.thread_id) return true;
    assert(inst.current != NULL);

    return inst.current->state == SRE_COROUTINESTATE_RUNNING;
}

bool sre_coroutinesuspend()
{
    sre_coroutine* current;
    sre_coroutinesuspendEx(&current);
    return current != NULL;
}

void* sre_coroutinesuspendEx(sre_coroutine** current)
{
    static sre_coroutine* dummy;
    if (!current) current = &dummy;

    if (SDL_ThreadID() != inst.thread_id)
    {
        *current = NULL;
        return NULL;
    }
    assert(inst.current != NULL);
    *current = inst.current;

    if (inst.current->state == SRE_COROUTINESTATE_CANCELLED) goto END_FUNC;
    inst.current->state = SRE_COROUTINESTATE_SUSPENDED;
    inst.current->resume_tick = 0;
    sys_coroutineswitch(&inst.thread_native);

    END_FUNC:
    return inst.current->data;
}

bool sre_coroutineyield(sre_timeStamp time)
{
    if (SDL_ThreadID() != inst.thread_id) return false;
    assert(inst.current != NULL);

    if (inst.current->state == SRE_COROUTINESTATE_CANCELLED) return true;
    if (time > 0) // Passing a `time` of zero (or a value lower than 0 if you'd like to shake your head and risk an assertion)
                  // will simply switch execution to other coroutines
    {
        inst.current->state = SRE_COROUTINESTATE_SUSPENDED;
        inst.current->resume_tick = (unsigned long)(SDL_GetTicks64() + (Uint64)(time * 1000));
    }
    sys_coroutineswitch(&inst.thread_native);

    return true;
}

sre_coroutineState sre_coroutinestate(const sre_coroutine* coroutine)
{
    return !coroutine ? SRE_COROUTINESTATE_INVALID : coroutine->state;
}

void sre_coroutinecancel(sre_coroutine* coroutine)
{
    if (!coroutine) return;

    coroutine->state = SRE_COROUTINESTATE_CANCELLED;
    
    // No need to cleanup anything else, the coroutine will automatically be cleaned up :)
    // Maybe I'll need to wait until the coroutine cleans up?
}

void COROUTINE_CALL coroutine_entry(void* _data)
{
    register coroutine_data* data = _data;

    sre_coroutineFunction func = data->func;
    sre_coroutineState* stateptr = data->stateptr;
    void* userdata = data->userdata;
    sre_delete(data);

    func(userdata);

    *stateptr = SRE_COROUTINESTATE_CANCELLED;
    sys_coroutineswitch(&inst.thread_native);

    // Right now when a coroutine finishes without closing then it reaches this line
    assert("This location should NOT be reached" && NULL);
}