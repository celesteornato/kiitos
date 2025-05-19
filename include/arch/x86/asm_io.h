#ifndef X86_ASM_IO_H_
#define X86_ASM_IO_H_

#include <stdint.h>

// NOLINTBEGIN(hicpp-no-assembler)

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile("outb %b0, %1" : : "a"(val), "Nd"(port) : "memory");
}
static inline void outw(uint16_t port, uint16_t val)
{
    __asm__ volatile("outl %0, %1" ::"a"(val), "d"(port));
}
static inline void outl(uint16_t port, uint32_t val)
{
    __asm__ volatile("outl %0, %1" ::"a"(val), "d"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret = 0;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}
static inline uint16_t inw(uint16_t port)
{
    uint16_t ret = 0;
    __asm__ volatile("inw %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}
static inline uint32_t inl(uint16_t port)
{
    uint32_t ret = 0;
    __asm__ volatile("inl %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

static inline void io_wait(void) { outb(0x80, 0); }
// NOLINTEND(hicpp-no-assembler)
#endif // X86_ASM_IO_H_
