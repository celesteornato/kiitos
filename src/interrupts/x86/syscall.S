.macro pusha_without_rax
    push %rcx
    push %rdx
    push %rbx
    push %rbp
    push %rsi
    push %rdi
.endm

.macro popa_without_rax
    pop %rdi
    pop %rsi
    pop %rbp
    pop %rbx
    pop %rdx
    pop %rcx
.endm


.globl isr_syscall


// Takes DI as a parameter, which internal_syscall will also see.
// RAX will be changed, this is expected.
// Called in the PIC.
isr_syscall:
    pusha_without_rax
    cld
    call internal_syscall
    popa_without_rax
    iretq
