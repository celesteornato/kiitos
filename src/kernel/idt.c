#include "idt.h"
#include "../kernel/kiitkio.h"
#include <stdint.h>

#define GDT_OFFSET_KERNEL_CODE 0x8
#define IDT_MAX_DESCRIPTORS 256

struct idt_entry {
  uint16_t isr_low;   // The lower 16 bits of the ISR's address
  uint16_t kernel_cs; // The GDT segment selector that the CPU will load into CS
                      // before calling the ISR
  uint8_t ist; // The IST in the TSS that the CPU will load into RSP; set to
               // zero for now
  uint8_t attributes; // Type and attributes; see the IDT page
  uint16_t
      isr_mid; // The higher 16 bits of the lower 32 bits of the ISR's address
  uint32_t isr_high; // The higher 32 bits of the ISR's address
  uint32_t reserved; // Set to zero
} __attribute__((packed));

struct idtr {
  uint16_t limit;
  uint64_t base;
} __attribute__((packed));

__attribute__((aligned(0x10))) static struct idt_entry g_idt[256];

static struct idtr g_idtr;

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
  struct idt_entry *descriptor = &g_idt[vector];
  descriptor->isr_low = (uint64_t)isr & 0xFFFF;
  descriptor->kernel_cs = GDT_OFFSET_KERNEL_CODE;
  descriptor->ist = 0;
  descriptor->attributes = flags;
  descriptor->isr_mid = ((uint64_t)isr >> 16) & 0xFFFF;
  descriptor->isr_high = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
  descriptor->reserved = 0;
}

static char vectors[IDT_MAX_DESCRIPTORS];

extern void *isr_stub_table[];

void load_idt() {

  g_idtr.base = (uintptr_t)&g_idt[0];
  g_idtr.limit = (uint16_t)sizeof(struct idt_entry) * IDT_MAX_DESCRIPTORS - 1;

  for (uint8_t vector = 0; vector < 32; vector++) {
    idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
    vectors[vector] = 1;
  }

  __asm__ volatile("lidt %0" : : "m"(g_idtr)); // load the new IDT
  __asm__ volatile("sti");                     // set the interrupt flag
}
