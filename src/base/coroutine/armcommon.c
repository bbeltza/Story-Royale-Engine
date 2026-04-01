#define COROUTINE_STACKVAR stack
#define COROUTINE_STACKSIZE stacksize

static void coroutine_entry(void* data);
typedef void* reg_t;

typedef struct coroutine_arm
{
    reg_t lr;
    reg_t sp;

    #ifdef __aarch64__
        reg_t xp[10]; // x19..x28
        reg_t x0; // Argument register (we need it to pass in `data`)
#else
        reg_t r[13];
    #endif

    void* stack;
    size_t stacksize;
} coroutine_native;

static bool sys_coroutinecreate(coroutine_native* coroutine, const coroutine_data* data)
{
    uintptr_t sp = (uintptr_t)coroutine->stack + coroutine->stacksize;
    sp = (sp & -16L);
        
    coroutine->lr = coroutine_entry;
    coroutine->sp = (reg_t)sp;
    #ifdef __aarch64__
        coroutine->x0 = (reg_t)data;
    #else
        coroutine->r[0] = (reg_t)data;
    #endif


    return true;
}

static bool sys_coroutinepoolsetup(coroutine_native* pool)
{
    (void)pool;
    return true;
}