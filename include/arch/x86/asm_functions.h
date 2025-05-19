#ifndef X86_ASM_FUNCS_H_
#define X86_ASM_FUNCS_H_

#include <arch/x86/asm_io.h>
#include <stdint.h>

// NOLINTBEGIN(hicpp-no-assembler)

/* Halt and catch fire, fully halts the kernel */
[[noreturn]] static inline void hcf(void)
{
    for (;;)
    {
        __asm__ volatile("hlt");
    }
}

static inline void interrupt_disable(void) { __asm__ volatile("cli"); }

static inline void interrupt_enable(void) { __asm__ volatile("sti"); }

// NOLINTEND(hicpp-no-assembler)

#endif // X86_ASM_FUNCS_H_
