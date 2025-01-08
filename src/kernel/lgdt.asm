bits 64
section .data
gdtr:
    dw 0
    dq 0

section .text

global load_gdt
global reload_segments
load_gdt:
   MOV [gdtr], DI
   MOV [gdtr+2], RSI
   LGDT [gdtr]
   RET
reload_segments:
    ; Reload CS register:
    push 0x08                
    lea rax, [rel .reload_CS]
    push rax                 
    retfq                    
.reload_CS:
    ; Reload data segment registers
    mov   ax, 0x10
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    mov   ss, ax
    ret
