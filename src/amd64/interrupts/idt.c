#include "amd64/interrupts/idt.h"
#include "amd64/interrupts/handlers.h"
#include <stdint.h>

enum gate_desc_flag : uint16_t {
    IST_0 = 1 << 0,
    IST_1 = 1 << 1,
    IST_2 = 1 << 2,
    TYPE_0 = 1 << 8,
    TYPE_1 = 1 << 9,
    TYPE_2 = 1 << 10,
    TYPE_3 = 1 << 11,
    INTERRUPT = TYPE_1 | TYPE_2 | TYPE_3,
    TRAP = TYPE_0 | TYPE_1 | TYPE_2 | TYPE_3,
    DPL_0 = 1 << 13,
    DPL_1 = 1 << 14,
    PRESENT = 1 << 15,
};

struct [[gnu::packed]] interrupt_descriptor {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint16_t flags;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
};

static struct interrupt_descriptor idt[IDT_LENGtH];

static struct [[gnu::packed]] {
    uint16_t limit;
    struct interrupt_descriptor *base;
} idtr = {.limit = sizeof(idt) - 1, .base = idt};

static void idt_set_descriptor(ptrdiff_t idx, void (*isr)(void), uint16_t flags)
{
    struct interrupt_descriptor *descriptor = &idt[idx];
    uint64_t isr_ptr = (uint64_t)isr;
    descriptor->isr_low = isr_ptr & 0xFFFFU;
    descriptor->kernel_cs = 0x08U;
    descriptor->flags = flags;
    descriptor->isr_mid = (uint16_t)((isr_ptr >> 16U) & 0xFFFFU);
    descriptor->isr_high = (uint32_t)((isr_ptr >> 32U) & 0xFFFFFFFFU);
    descriptor->reserved = 0;
}

void idt_init(void)
{
    for (ptrdiff_t i = 0; i < 32; ++i)
    {
        idt_set_descriptor(i, except_fatal, INTERRUPT | PRESENT);
    }

    __asm__ volatile("lidt %0" ::"m"(idtr));
    // sti is called in the arch_init function (amd64.c's)
}
