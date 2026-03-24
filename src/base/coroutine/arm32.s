#include "armcommon.c"

__attribute__((naked)) static void sys_coroutineswitch(coroutine_native* coroutine, coroutine_native* current)
{
    __asm(
        "STR LR, [R1, #0*4]\n\t"
        "STR SP, [R1, #1*4]\n\t"
        "STR R0, [R1, #2*4]\n\t"
        "STR R1, [R1, #3*4]\n\t"
        "STR R2, [R1, #4*4]\n\t"
        "STR R3, [R1, #5*4]\n\t"
        "STR R4, [R1, #6*4]\n\t"
        "STR R5, [R1, #7*4]\n\t"
        "STR R6, [R1, #8*4]\n\t"
        "STR R7, [R1, #9*4]\n\t"
        "STR R8, [R1, #10*4]\n\t"
        "STR R9, [R1, #11*4]\n\t"
        "STR R10, [R1, #12*4]\n\t"
        "STR R11, [R1, #13*4]\n\t"
        "STR R12, [R1, #14*4]\n\t"

        // Context switch, pretty simple, since the return address is not stored in the stack but instead into a separate register
        "LDR LR, [R0, #0*4]\n\t"
        "LDR SP, [R0, #1*4]\n\t"
        "LDR R1, [R0, #3*4]\n\t"
        "LDR R2, [R0, #4*4]\n\t"
        "LDR R3, [R0, #5*4]\n\t"
        "LDR R4, [R0, #6*4]\n\t"
        "LDR R5, [R0, #7*4]\n\t"
        "LDR R6, [R0, #8*4]\n\t"
        "LDR R7, [R0, #9*4]\n\t"
        "LDR R8, [R0, #10*4]\n\t"
        "LDR R9, [R0, #11*4]\n\t"
        "LDR R10, [R0, #12*4]\n\t"
        "LDR R11, [R0, #13*4]\n\t"
        "LDR R12, [R0, #14*4]\n\t"

        "LDR R0, [R0, #2*4]\n\t"

        "BX LR"
    );
}

