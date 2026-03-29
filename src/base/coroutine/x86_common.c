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
    void* stack;
    size_t stacksize;
} coroutine_native;

#define COROUTINE_STACKVAR stack
#define COROUTINE_STACKSIZE stacksize

static bool sys_coroutinecreate(coroutine_native* coroutine, const coroutine_data* data)
{
    uintptr_t sp = (uintptr_t)coroutine->stack + coroutine->stacksize;
        
    coroutine->regs.rip = coroutine_entry;
    #ifdef __x86_64__
        // Vector instructions crash when rsp is not aligned correctly, it broke me out of my entire mind
        sp = (sp & -16)  -8;
        coroutine->regs.rsp = (reg_t)sp;
        coroutine->regs.rdi = (void*)data;
    #elif __i386__
        // Arguments in x32 are passed in the stack, do that of course
        *(void**)sp = (void*)data;
        coroutine->regs.rsp = (reg_t)(sp - 4);
    #endif

    return true;
}

static bool sys_coroutinepoolsetup(coroutine_native* pool)
{
    (void)pool;
    return true;
}