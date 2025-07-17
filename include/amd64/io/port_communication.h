#ifndef PORT_COMMUNICATION_H_
#define PORT_COMMUNICATION_H_

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port) : "memory");
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
#endif // PORT_COMMUNICATION_H_
