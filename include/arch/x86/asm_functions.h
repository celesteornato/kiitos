#ifndef X86_ASM_FUNCS_H_
#define X86_ASM_FUNCS_H_

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

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret = 0;
    __asm__ volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

static inline void io_wait(void) { outb(0x80, 0); }

// NOLINTEND(hicpp-no-assembler)

#endif // X86_ASM_FUNCS_H_
