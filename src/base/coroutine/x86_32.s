#include "x86_common.c"

__attribute__((naked)) static void sys_coroutineswitch(coroutine_native* coroutine, coroutine_native* current)
{
    // Save current context
    __asm__(
        "mov 2*4(%esp), %edi\n\t"
        // `current` aka %esi might be NULL, check for that
        "test %edi, %edi\n\t"
        "jz CONTEXT_SWITCH\n\t"

        // Save current state, the register orders are in the `r_general` struct
        
        "mov %eax, 1*4(%edi)\n\t"
        // Set `eip` in `current` as the return addres in %esp
        "mov (%esp), %eax\n\t"
        "mov %eax, 0*4(%edi)\n\t"

        // Set `esp`, popped from the return address
        "lea -4(%esp), %eax\n\t"
        "mov %eax, 5*4(%edi)\n\t"

        // Save the rest of the registers
        "mov %ebx, 2*4(%edi)\n\t"
        "mov %ecx, 3*4(%edi)\n\t"
        "mov %edx, 4*4(%edi)\n\t"
        "mov %ebp, 6*4(%edi)\n\t"
        "mov %esi, 7*4(%edi)\n\t"
        "mov %edi, 8*4(%edi)\n\t"

        // Perform context switch
        "CONTEXT_SWITCH:\n\t"
        
        "mov 1*4(%esp), %edi\n\t"

        "mov 1*4(%edi), %eax\n\t"
        "mov 2*4(%edi), %ebx\n\t"
        "mov 3*4(%edi), %ecx\n\t"
        "mov 4*4(%edi), %edx\n\t"

        "mov 5*4(%edi), %esp\n\t"
        "mov 6*4(%edi), %ebp\n\t"

        // Push `eip` from `coroutine`, it'll jump there once the function hits `ret`, aka returns
        "push 0*4(%edi)\n\t"

        // Finally...
        "mov 7*4(%edi), %esi\n\t"
        "mov 8*4(%edi), %edi\n\t"

        "ret\n\t"
    );
}