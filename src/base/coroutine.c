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
static void sys_coroutineswitch(coroutine_native* coroutine);

struct sre_coroutine
{
    sre_coroutine* next;
    sre_coroutineState state;

    coroutine_native native;
};

struct coroutine_TLS
{
    const void* magic;
    sre_coroutine* coroutine;
};

static const char cTLSmagic;

static int SDLCALL poolthread_proc(void* head)
{
    // Thread will contain two TLS values:
        // - The native coroutine data for this thread (The state or currently `pool_native`)
        // - The current running coroutine, it's usefull in order to retrieve the current coroutine
        //   in functions like `sre_coroutinesuspend()` and `sre_coroutineyield`

        // - I might just add another TLS containing the magic address (&cTLSmagic) and then assume the other ids are
        //   what I expect

    SDL_TLSID current_coroutineTLS = SDL_TLSCreate();
    SDL_TLSID pool_nativeTLS = SDL_TLSCreate();

    {
        struct coroutine_TLS* c_TLS = sre_new(sizeof(struct coroutine_TLS));
        c_TLS->magic = &cTLSmagic;
        c_TLS->coroutine = NULL;
        SDL_TLSSet(current_coroutineTLS, c_TLS, sre_delete);
    }

    // Let's print the tls ids first. Okay so it's supposed to be 4 and 5
    LOG("%u %u", current_coroutineTLS, pool_nativeTLS);

    // Maybe it is not always 4 and 5 on every system?
    assert(current_coroutineTLS == 4);
    assert(pool_nativeTLS == 5);

    coroutine_native pool_native;
    memset(&pool_native, 0, sizeof(coroutine_native));
    SDL_TLSSet(pool_nativeTLS, &pool_native, NULL);

    while (true)
    {
        sre_coroutine* current = *(sre_coroutine**)head;
        while (current)
        {
            assert(current->state != SRE_COROUTINESTATE_INVALID);
            if (current->state == SRE_COROUTINESTATE_SUSPENDED) goto ENDLOOP;

            // Code to perform the OS switch
                sys_coroutinepoolsetup(&pool_native);
                sys_coroutineswitch(&current->native);
            //

            ENDLOOP:
            current = current->next;
        }

        SDL_Delay(1);
    }

    return 0;
}

static SDL_Thread* pool_thrd;
static sre_coroutine* coroutine_head;

bool sre_coroutinecoreinit()
{
    assert(pool_thrd == NULL /* Cannot call sre_coroutinecoreinit() twice!! */);
    
    pool_thrd = SDL_CreateThread(poolthread_proc, "Coroutine pool", &coroutine_head);
    if (!pool_thrd)
        return false;
    
    return true;
}

void sre_coroutinecorequit()
{
    if (!pool_thrd) return;
    SDL_DetachThread(pool_thrd);
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
    coroutine->next = coroutine_head;
    coroutine_head = coroutine;

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
    struct coroutine_TLS* current = SDL_TLSGet(4);
    if (!current || current->magic != &cTLSmagic)
        return false;
    
    assert(current->coroutine != NULL);
    current->coroutine->state = SRE_COROUTINESTATE_SUSPENDED;

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