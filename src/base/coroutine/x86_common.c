static void coroutine_entry(void* data);

typedef void* reg_t;

struct r_general
{
    reg_t rip;
    reg_t rax;
    reg_t rbx;
    reg_t rcx;
    reg_t rdx;
    reg_t rsp;
    reg_t rbp;
    reg_t rsi;
    reg_t rdi;

    #ifdef __x86_64__
        reg_t r8;
        reg_t r9;
        reg_t r10;
        reg_t r11;
        reg_t r12;
        reg_t r13;
        reg_t r14;
        reg_t r15;
    #endif
};

typedef struct coroutine_x86_64
{
    struct r_general regs;

    char* stack;
} coroutine_native;

static bool sys_coroutinecreate(coroutine_native* coroutine, const coroutine_data* data)
{
    const size_t stacksize = 4096*1024;

    coroutine->stack = sre_new(stacksize);
    uintptr_t sp = (uintptr_t)coroutine->stack + stacksize;
    #ifdef __x86_64
        // Vector instructions crash when rsp is not aligned correctly, it broke me out of my entire mind
        sp = (sp & -16)  -8;
    #endif

    coroutine->regs.rip = coroutine_entry;
    coroutine->regs.rsp = (reg_t)sp;
    coroutine->regs.rdi = (void*)data;

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