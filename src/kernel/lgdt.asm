bits 64
section .data
ALIGN 16

section .text
global load_gdt
global reload_segments
load_gdt:
    cli
    lgdt [rdi] ; rdi=1st arg, gdtr ptr
    ret

reload_segments:
    ; Reload CS register:
    push 0x08                 
    lea rax, [rel .reload_CS] 
    push rax                  
    retfq                     
.reload_CS:
    hlt
    mov   ax, 0x10 ; 0x10 is a stand-in for your data segment
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    mov   ss, ax
    ret
