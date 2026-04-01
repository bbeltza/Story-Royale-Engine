#include "x86_common.c"

__attribute__((naked))
 static void sys_coroutineswitch(coroutine_native* coroutine, coroutine_native* current)
{
    // Save current context
    __asm__(
        // `current` aka %rsi might be NULL, check for that
        "test %rsi, %rsi\n\t"
        "jz CONTEXT_SWITCH\n\t"

        // Save current state, the register orders are in the `r_general` struct
        
        "mov %rax, 1*8(%rsi)\n\t"
        // Set `rip` in `current` as the return addres in %rsp
        "mov (%rsp), %rax\n\t"
        "mov %rax, 0*8(%rsi)\n\t"

        // Set `rsp`, popped from the return address
        "lea 8(%rsp), %rax\n\t"
        "mov %rax, 5*8(%rsi)\n\t"

        // Save the rest of the registers
        "mov %rbx, 2*8(%rsi)\n\t"
        "mov %rcx, 3*8(%rsi)\n\t"
        "mov %rdx, 4*8(%rsi)\n\t"
        "mov %rbp, 6*8(%rsi)\n\t"
        "mov %rsi, 7*8(%rsi)\n\t"
        "mov %rdi, 8*8(%rsi)\n\t"
        "mov %r8, 9*8(%rsi)\n\t"
        "mov %r9, 10*8(%rsi)\n\t"
        "mov %r10, 11*8(%rsi)\n\t"
        "mov %r11, 12*8(%rsi)\n\t"
        "mov %r12, 13*8(%rsi)\n\t"
        "mov %r13, 14*8(%rsi)\n\t"
        "mov %r14, 15*8(%rsi)\n\t"
        "mov %r15, 16*8(%rsi)\n\t"

        // Perform context switch
        "CONTEXT_SWITCH:\n\t"

        "mov 1*8(%rdi), %rax\n\t"
        "mov 2*8(%rdi), %rbx\n\t"
        "mov 3*8(%rdi), %rcx\n\t"
        "mov 4*8(%rdi), %rdx\n\t"

        "mov 5*8(%rdi), %rsp\n\t"
        "mov 6*8(%rdi), %rbp\n\t"

        "mov 9*8(%rdi), %r8\n\t"
        "mov 10*8(%rdi), %r9\n\t"
        "mov 11*8(%rdi), %r10\n\t"
        "mov 12*8(%rdi), %r11\n\t"
        "mov 13*8(%rdi), %r12\n\t"
        "mov 14*8(%rdi), %r13\n\t"
        "mov 15*8(%rdi), %r14\n\t"
        "mov 16*8(%rdi), %r15\n\t"

        // Push `rip` from `coroutine`, it'll jump there once the function hits `ret`, aka returns
        "push 0*8(%rdi)\n\t"

        // Finally...
        "mov 7*8(%rdi), %rsi\n\t"
        "mov 8*8(%rdi), %rdi\n\t"

        "ret\n\t"
    );
}

