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

#if !defined(_MSC_VER) && defined(__x86_64__)
    #include "coroutine/x86_64.c"
#elif defined(_WIN32)
    #include "coroutine/win32.c"
#elif defined(HAVE_UCONTEXT_H)
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

static struct coroutine_instance* current_instance;

sre_coroutine* sre_coroutinecreate(bool suspended, sre_coroutineFunction function, void* userdata)
{
    while (!current_instance)
        (void)0;
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
    if (current_instance->end)
    {
        current_instance->end->next = coroutine;
    }
    else
    {
        assert(current_instance->head == NULL);
        current_instance->head = coroutine;
    }
    current_instance->end = coroutine;

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
    if (SDL_ThreadID() != current_instance->thread_id) return true;
    assert(current_instance->current != NULL);

    return current_instance->current->state == SRE_COROUTINESTATE_RUNNING;
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

    if (SDL_ThreadID() != current_instance->thread_id)
    {
        *current = NULL;
        return NULL;
    }
    assert(current_instance->current != NULL);
    *current = current_instance->current;

    if (current_instance->current->state == SRE_COROUTINESTATE_CANCELLED) goto END_FUNC;
    current_instance->current->state = SRE_COROUTINESTATE_SUSPENDED;
    current_instance->current->resume_tick = 0;
    sys_coroutineswitch(&current_instance->thread_native);

    END_FUNC:
    return current_instance->current->data;
}

bool sre_coroutineyield(sre_timeStamp time)
{
    if (SDL_ThreadID() != current_instance->thread_id) return false;
    assert(current_instance->current != NULL);

    if (current_instance->current->state == SRE_COROUTINESTATE_CANCELLED) return true;
    if (time > 0) // Passing a `time` of zero (or a value lower than 0 if you'd like to shake your head and risk an assertion)
                  // will simply switch execution to other coroutines
    {
        current_instance->current->state = SRE_COROUTINESTATE_SUSPENDED;
        current_instance->current->resume_tick = (unsigned long)(SDL_GetTicks64() + (Uint64)(time * 1000));
    }
    sys_coroutineswitch(&current_instance->thread_native);

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
    sys_coroutineswitch(&current_instance->thread_native);

    assert("This location should NOT be reached" && NULL);
}

void _coroutine_coreinit(void* running)
{
    struct coroutine_instance instance;
    memset(&instance, 0, sizeof(instance));
    current_instance = &instance;

    instance.thread_id = SDL_ThreadID();
    sys_coroutinepoolsetup(&instance.thread_native);

    while (*(bool*)running)
    {
        sre_coroutine* prev = NULL;
        instance.current = instance.head;
        while (instance.current)
        {
            assert(instance.current->state != SRE_COROUTINESTATE_INVALID);
            if (instance.current->state == SRE_COROUTINESTATE_SUSPENDED)
            {
                if (instance.current->resume_tick && instance.current->resume_tick <= SDL_GetTicks64())
                    instance.current->state = SRE_COROUTINESTATE_RUNNING;
                else
                    goto ENDLOOP;
            }

            // Code to perform the context switch
                sys_coroutineswitch(&instance.current->native);
                if (instance.current->state == SRE_COROUTINESTATE_CANCELLED)
                {
                    sre_coroutine* curr = instance.current;
                    if (prev)
                        prev->next = curr->next;
                    else
                        instance.head = curr->next;

                    if (curr == instance.end)
                        instance.end = prev;
                    
                    instance.current = curr->next;

                    sys_coroutineclose(&curr->native);
                    sre_delete(curr);

                    continue;
                }
            //

            ENDLOOP:
            prev = instance.current;
            instance.current = instance.current->next;
        }

        SDL_Delay(1);
    }

    // Close coroutine engine, cleanup remaining coroutines
    while (instance.head)
    {
        sre_coroutine* curr = instance.head;
        instance.head = instance.head->next;
        
        instance.current = curr;
        curr->state = SRE_COROUTINESTATE_CANCELLED;
        sys_coroutineswitch(&curr->native);
        sre_delete(curr);
    }
}