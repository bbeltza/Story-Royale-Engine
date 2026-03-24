static void coroutine_entry(void* data);
typedef void* reg_t;

typedef struct coroutine_arm
{
    reg_t lr;
    reg_t sp;

    reg_t r[13];

    char* stack;
} coroutine_native;

static void sys_coroutineswitch(coroutine_native* coroutine, coroutine_native* current)
{
    __asm(
        "STR LR, [R1, #0]\n\t"
        "STR SP, [R1, R4]\n\t"
        "STR R0, [R1, #8]\n\t"
        "STR R1, [R1, #12]\n\t"
        "STR R2, [R1, #16]\n\t"
        "STR R3, [R1, #20]\n\t"
        "STR R4, [R1, #24]\n\t"
        "STR R5, [R1, #28]\n\t"
        "STR R6, [R1, #32]\n\t"
        "STR R7, [R1, #36]\n\t"
        "STR R8, [R1, #40]\n\t"
        "STR R9, [R1, #44]\n\t"
        "STR R10, [R1, #48]\n\t"
        "STR R11, [R1, #52]\n\t"
        "STR R12, [R1, #56]\n\t"
    );
}

static bool sys_coroutinecreate(coroutine_native* coroutine, const coroutine_data* data)
{
    const size_t stacksize = 4096*1024;

    coroutine->stack = sre_new(stacksize);
    uintptr_t sp = (uintptr_t)coroutine->stack + stacksize;
        
    coroutine->lr = coroutine_entry;
    coroutine->sp = (reg_t)sp;
    coroutine->r[0] = data;

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