#include "idt.h"

#define IDT_SIZE 256


static char vectors[IDT_SIZE];

extern void* isr_stub_table[];

static struct idtr g_idtr;
__attribute__((
    aligned(0x10))) static struct interrupt_descriptor g_idt[IDT_SIZE];

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
  struct interrupt_descriptor *descriptor = &g_idt[vector];

  descriptor->isr_low = (uint64_t)isr & 0xFFFF;
  descriptor->kernel_cs = 0x08;
  descriptor->ist = 0;
  descriptor->attributes = flags;
  descriptor->isr_mid = ((uint64_t)isr >> 16) & 0xFFFF;
  descriptor->isr_high = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
  descriptor->reserved = 0;
}

void idt_init() {
    g_idtr.base = (uintptr_t)&g_idt[0];
    g_idtr.limit = (uint16_t)sizeof(struct interrupt_descriptor) * IDT_SIZE - 1;

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = 1;
    }

    __asm__ volatile ("lidt %0" : : "m"(g_idtr)); // load the new IDT
    __asm__ volatile ("sti"); // set the interrupt flag
}
