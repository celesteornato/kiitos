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


.globl syscall
.globl isr_syscall


// Function that makes it easier to trigger a syscall, called in the kernel code.
// In the corresponding header, takes a parameter (%di), which will be passed to isr_syscall
syscall:
    int $0x80
    ret

// Takes DI as a parameter, which internal_syscall will also see.
// RAX will be changed, this is expected.
// Called in the PIC.
isr_syscall:
    pusha_without_rax
    cld
    call internal_syscall
    popa_without_rax
    iretq
