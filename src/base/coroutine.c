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
    
typedef struct coroutine_data coroutine_data;

#if defined(_WIN32)
    #include "coroutine/win32.c"
#elif defined(__arm__)
    #include "coroutine/arm32.s"
#elif defined(__aarch64__)
    #include "coroutine/arm64.s"
#elif !defined(_MSC_VER) && defined(__x86_64__)
    #include "coroutine/x86_64.s"
#elif !defined(_MSC_VER) && defined(__i386__)
    #include "coroutine/x86_32.s"
#elif defined(SRE_HAVE_UCONTEXT_H)
    #include "coroutine/ucontext.c"
#else
    #include "coroutine/dummy.c"
    #error "Make an implementation!! Lazy..."
#endif

#ifndef COROUTINE_CALL
    #define COROUTINE_CALL
#endif

/* `sys_` functions */
#ifdef COROUTINE_STACKVAR
#ifndef COROUTINE_STACKSIZE
    #error "Need to define `COROUTINE_STACKSIZE` if you're willing to use COROUTINE_STACKVAR"
#endif
    #include "coroutine/stack.c"
    // Allocate the coroutine's stack (if needed, it can be shared between multiple implementations)
    // `COROUTINE_STACKVAR` is the member from `coroutine_native` in which the stack is stored, it has to be a pointer
    // `COROUTINE_STACKSIZE` has to then be defined, to be a member holding the stack size
    static bool sys_coroutinestack(void** base, size_t* size);
#endif
// Setup to the coroutine (using CreateFiber on win32 for example)
static bool sys_coroutinecreate(coroutine_native* coroutine, const coroutine_data* data);
// Setup the thread pool context coroutine (On win32 it will just call SwitchThreadToFiber once if what's in `pool` is NULL)
// `pool` is automatically initialized to `NULL` for the first time
static bool sys_coroutinepoolsetup(coroutine_native* pool);
// Switch to the following coroutine
static void sys_coroutineswitch(coroutine_native* coroutine, coroutine_native* current);

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
    while (!*(struct coroutine_instance* volatile*)(&current_instance))
        (void)0;
    sre_coroutine* coroutine = sre_newclear(sizeof(sre_coroutine));

    coroutine_data* data = sre_new(sizeof(coroutine_data));
    data->func = function;
    data->userdata = userdata;
    data->stateptr = &coroutine->state;



    if (
#ifdef COROUTINE_STACKVAR
            !sys_coroutinestack(&coroutine->native.COROUTINE_STACKVAR, &coroutine->native.COROUTINE_STACKSIZE) ||
#endif
            !sys_coroutinecreate(&coroutine->native, data)
            )
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
    if (!current_instance) return false;
    if (SDL_ThreadID() != current_instance->thread_id) return false;
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
    sys_coroutineswitch(&current_instance->thread_native, &current_instance->current->native);

    END_FUNC:
    return current_instance->current->data;
}

bool sre_coroutineyield(sre_timeStamp time)
{
    if (SDL_ThreadID() != current_instance->thread_id) return false;
    assert(current_instance->current != NULL);

    if (current_instance->current->state == SRE_COROUTINESTATE_CANCELLED) return true;
    if (time > 0) // Passing a `time` of zero (or a value lower than 0 if you'd like to shake your head and risk an assertion)
                  // will simply just switch execution to other coroutines
    {
        current_instance->current->state = SRE_COROUTINESTATE_SUSPENDED;
        current_instance->current->resume_tick = (unsigned long)(SDL_GetTicks64() + (Uint64)(time * 1000));
    }
    sys_coroutineswitch(&current_instance->thread_native, &current_instance->current->native);

    return true;
}

sre_coroutineState sre_coroutinestate(const sre_coroutine* coroutine)
{
    return !coroutine ? SRE_COROUTINESTATE_INVALID : coroutine->state;
}

void sre_coroutinecancel(sre_coroutine* coroutine)
{
    if (!current_instance) return;
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
    sys_coroutineswitch(&current_instance->thread_native, NULL);

    assert("This location should NOT be reached" && NULL);
}


#ifdef ANDROID
    #include <jni.h>
    #include <SDL_system.h>
    #include <pthread.h>

    // JNI stack switcher warpper, only for aarch64 for now. Need to incorporate the other functions
    static __attribute__((naked)) jint __jni_attach(jint (*fun)(JavaVM*, JNIEnv**, void*), JavaVM* vm, JNIEnv** env, void* bp)
    {
        __asm(
                "mov x8, x0\n\t"
                "mov x0, x1\n\t"
                "mov x1, x2\n\t"
                "mov x2, #0\n\t"

                "mov x4, sp\n\t"
                "mov sp, x3\n\t"
                "sub sp, sp, #16\n\t"
                "stp x4, lr, [sp]\n\t"

                "blr x8\n\t"

                "ldp x2, lr, [sp]\n\t"
                "mov sp, x2\n\t"
                "ret"
                );
    }

    // JNI setup function before switching to context! (Still unfinished)
    static void _coroutine_setup_jni(const coroutine_native* ctx)
    {
        static JavaVM* vm;
        JNIEnv* env;
        if (!vm)
        {
            JNIEnv* env = SDL_AndroidGetJNIEnv();
            (*env)->GetJavaVM(env, &vm);
        }

        (*vm)->DetachCurrentThread(vm);

        pthread_t self = pthread_self();
        void** base = (void**)(self+32);
        size_t* size = (size_t*)(self+40);
        *base = ctx->COROUTINE_STACKVAR;
        *size = ctx->COROUTINE_STACKSIZE;

        #if defined(__x86_64__)
            #define __sp ctx->rsp
        #elif defined(i386)
            #define __sp ctx->esp
        #elif defined(__arm__) || defined(__aarch64__)
            #define __sp ctx->sp
        #endif

        __jni_attach((*vm)->AttachCurrentThread, vm, &env, __sp);

        #undef __sp
    }
#endif

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
                //__debugbreak();
                #ifdef ANDROID
                    _coroutine_setup_jni(&instance.current->native);
                #endif

                sys_coroutineswitch(&instance.current->native, &instance.thread_native);
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
        sys_coroutineswitch(&curr->native, &instance.thread_native);
        sre_delete(curr);
    }
    current_instance = NULL;
}