.macro pushaq
    push %rax
    push %rcx
    push %rdx
    push %rbx
    push %rbp
    push %rsi
    push %rdi
.endm

.macro popaq
    pop %rdi
    pop %rsi
    pop %rbp
    pop %rbx
    pop %rdx
    pop %rcx
    pop %rax
.endm

.macro except n
    .globl isr_\n
    isr_\n:
        pushaq
        cld    // C code following the sysV ABI requires DF to be clear on function entry
        mov $\n , %di
        call exception_handler
        popaq
        iretq
.endm

.macro unhandled n
    .globl isr_\n
    isr_\n:
        pushaq
        cld    // C code following the sysV ABI requires DF to be clear on function entry
        mov $\n , %di
        call unhandled
        popaq
        iretq
.endm

.macro handled name
    .globl isr_\name
    .extern internal_\name
    isr_\name:
        cli
        pushaq
        cld    // C code following the sysV ABI requires DF to be clear on function entry
        call internal_\name
        popaq
        sti
        iretq
.endm

.extern exception_handler
.extern unhandled

// There are three kinds of interrupts: Handled, Unhandled, and Exceptions.
// Exceptions (0-32) ensue a kernel panic, specific behaviour is implemented in ../interrupts.c in the exception_handler() function.
// Unhandled means I haven't gotten to it, they ensue an non-fatal error message.
// Handled functions have their specific behaviour in internal_[functionanme] in ../interrupts.c
//
// Exceptions and Unhandled interrupts are numbered and not named, as the number corresponds "error code" for their generic function.
// As neither are desirable, this is not and issue.

except 0
except 1
except 2
except 3
except 4
except 5
except 6
except 7
except 8
except 9
except 10
except 11
except 12
except 13
except 14
except 15
except 16
except 17
except 18
except 19
except 20
except 21
except 22
except 23
except 24
except 25
except 26
except 27
except 28
except 29
except 30
except 31

handled clock
handled kbinp
handled mouse

unhandled 34
unhandled 35
unhandled 36
unhandled 37
unhandled 38
unhandled 39
unhandled 40
unhandled 41
unhandled 42
unhandled 43
