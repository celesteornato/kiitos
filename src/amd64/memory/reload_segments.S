.section .data
.GDTR:
    .word 0
    .quad 0

.section .text
.globl asm_reload_segments
asm_reload_segments:
    push $0x08
    leaq reload_cs, %rax
    push %rax
    lretq

reload_cs:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    ret
