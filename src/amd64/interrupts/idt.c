#include <amd64/debug/logging.h>
#include <amd64/interrupts/idt.h>
#include <fun/colors.h>
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

void (*(vectors[1]))(void) = {nullptr};

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
    constexpr uint8_t isr_table_len = sizeof(vectors) / sizeof(vectors[0]);
    idt_set_descriptor(0, vectors[0], 0x8E);

    __asm__ volatile("lidt %0" ::"m"(idtr));
    // sti is called in the arch_init function (amd64.c's)
}
