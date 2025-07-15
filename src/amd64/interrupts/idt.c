#include "amd64/interrupts/idt.h"
#include "amd64/interrupts/handlers.h"
#include <stdint.h>

struct [[gnu::packed]] interrupt_descriptor {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t ist;
    uint8_t flags;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
};

struct interrupt_descriptor idt[IDT_LENGtH];

struct [[gnu::packed]] {
    uint16_t limit;
    struct interrupt_descriptor *base;
} idtr = {.limit = sizeof(idt) - 1, .base = idt};

static void idt_set_descriptor(ptrdiff_t idx, void (*isr)(void), uint8_t flags)
{
    struct interrupt_descriptor *descriptor = &idt[idx];
    uint64_t isr_ptr = (uint64_t)isr;
    descriptor->isr_low = isr_ptr & 0xFFFFU;
    descriptor->kernel_cs = 0x08U;
    descriptor->ist = 0;
    descriptor->flags = flags;
    descriptor->isr_mid = (uint16_t)((isr_ptr >> 16U) & 0xFFFFU);
    descriptor->isr_high = (uint32_t)((isr_ptr >> 32U) & 0xFFFFFFFFU);
    descriptor->reserved = 0;
}

void idt_init(void)
{
    for (ptrdiff_t i = 0; i < 32; ++i)
    {
        idt_set_descriptor(i, except_fatal, 0x8E);
    }

    __asm__ volatile("lidt %0" ::"m"(idtr));
    // sti is called in the arch_init function (amd64.c's)
}
