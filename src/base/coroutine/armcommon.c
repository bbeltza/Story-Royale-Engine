static void coroutine_entry(void* data);
typedef void* reg_t;

typedef struct coroutine_arm
{
    reg_t lr;
    reg_t sp;

    #ifdef __aarch64__
        reg_t x[30];
    #else
        reg_t r[13];
    #endif

    char* stack;
} coroutine_native;

static bool sys_coroutinecreate(coroutine_native* coroutine, const coroutine_data* data)
{
    const size_t stacksize = 4096*1024;

    coroutine->stack = sre_new(stacksize);
    uintptr_t sp = (uintptr_t)coroutine->stack + stacksize;
    sp = (sp & -16L);
        
    coroutine->lr = coroutine_entry;
    coroutine->sp = (reg_t)sp;
    #ifdef __aarch64__
        coroutine->x[0] = (reg_t)data;
    #else
        coroutine->r[0] = (reg_t)data;
    #endif


    return true;
}

static void sys_coroutineclose(coroutine_native* coroutine)
{
    sre_delete(coroutine->stack);
}

static bool sys_coroutinepoolsetup(coroutine_native* pool)
{
    (void)pool;
    return true;
}