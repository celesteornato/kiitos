#include "idt.h"

#define IDT_MAX_SIZE 256
#define IDT_SIZE 32
#define KERNEL_C_OFFSET 0x08

static char vectors[IDT_MAX_SIZE];
extern void *isr_stub_table[];

static struct idtr g_idtr;
__attribute__((
    aligned(0x10))) static struct interrupt_descriptor g_idt[IDT_MAX_SIZE];

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
  struct interrupt_descriptor *descriptor = &g_idt[vector];
  *descriptor = (struct interrupt_descriptor){
      .isr_low = (uint64_t)isr & 0xFFFF,
      .kernel_cs = KERNEL_C_OFFSET,
      .ist = 0,
      .reserved_low = 0,
      .attributes = flags,
      .isr_mid = ((uint64_t)isr >> 16) & 0xFFFF,
      .isr_high = ((uint64_t)isr >> 32) & 0xFFFFFFFF,
      .reserved_high = 0,
  };
}

void idt_init() {
  g_idtr.base = (uintptr_t)&g_idt[0];
  g_idtr.limit =
      (uint16_t)sizeof(struct interrupt_descriptor) * IDT_MAX_SIZE - 1;

  for (uint8_t vector = 0; vector < IDT_SIZE; vector++) {
    idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
    vectors[vector] = 1;
  }

  __asm__ volatile("lidt %0" : : "m"(g_idtr)); // load the new IDT
  __asm__ volatile("sti");                     // set the interrupt flag
}
