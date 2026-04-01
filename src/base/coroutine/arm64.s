#include "armcommon.c"

__attribute__((naked)) static void sys_coroutineswitch(coroutine_native* coroutine, coroutine_native* current)
{
    __asm(
        "cbz x1, CONTEXT_SWITCH\n\t"

        // SP and LR first
        "mov x2, sp\n\t"
        "stp lr, x2, [x1, #0*8]\n\t"

        "stp x19, x20, [x1, #2*8]\n\t"
        "stp x21, x22, [x1, #4*8]\n\t"
        "stp x23, x24, [x1, #6*8]\n\t"
        "stp x25, x26, [x1, #8*8]\n\t"
        "stp x27, x28, [x1, #10*8]\n\t"
        "str x0, [x1, #12*8]\n\t"

        // Context switch!
     "CONTEXT_SWITCH:\n\t"
        "ldp lr, x1, [x0, #0*8]\n\t"
        "mov sp, x1\n\t"

        "ldp x19, x20, [x0, #2*8]\n\t"
        "ldp x21, x22, [x0, #4*8]\n\t"
        "ldp x23, x24, [x0, #6*8]\n\t"
        "ldp x25, x26, [x0, #8*8]\n\t"
        "ldp x27, x28, [x0, #10*8]\n\t"
        "ldr x0, [x0, #12*8]\n\t"

        "ret"
    );
}