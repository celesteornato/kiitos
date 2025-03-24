#include "idt.h"
#include "interrupts.h"
#include <stdbool.h>
#include <stdint.h>

#define IDT_MAX_DESCRIPTORS 256

struct interrupt_descriptor {
  uint16_t isr_low;
  uint16_t kernel_cs;
  uint8_t ist;
  uint8_t attributes;
  uint16_t isr_mid;
  uint32_t isr_high;
  uint32_t reserved;
} __attribute__((packed));

struct idtr {
  uint16_t limit;
  struct interrupt_descriptor *base;
} __attribute__((packed));

__attribute__((aligned(0x10))) static struct interrupt_descriptor idt[256];

static struct idtr idtr = {
    .base = idt,
    .limit = ((uint16_t)sizeof(struct interrupt_descriptor) * 256) - 1};

static bool vectors[IDT_MAX_DESCRIPTORS] = {0};

void (*isr_stub_table[])(void *) = {
    isr_stub0,  isr_stub1,  isr_stub2,  isr_stub3,  isr_stub4,  isr_stub5,
    isr_stub6,  isr_stub7,  isr_stub8,  isr_stub9,  isr_stub10, isr_stub11,
    isr_stub12, isr_stub13, isr_stub14, isr_stub15, isr_stub16, isr_stub17,
    isr_stub18, isr_stub19, isr_stub20, isr_stub21, isr_stub22, isr_stub23,
    isr_stub24, isr_stub25, isr_stub26, isr_stub27, isr_stub28, isr_stub29,
    isr_stub30, isr_stub31};

static void idt_set_descriptor(bool vector, void (*isr)(void *),
                               uint8_t flags) {
  struct interrupt_descriptor *descriptor = &idt[vector];
  uint64_t isr_ptr = (uint64_t)isr;
  descriptor->isr_low = isr_ptr & 0xFFFF;
  descriptor->kernel_cs = 0x08;
  descriptor->ist = 0;
  descriptor->attributes = flags;
  descriptor->isr_mid = (uint16_t)((isr_ptr >> 16) & 0xFFFF);
  descriptor->isr_high = (uint32_t)((isr_ptr >> 32) & 0xFFFFFFFF);
  descriptor->reserved = 0;
}

void idt_init(void) {
  const uint8_t isr_stub_len =
      sizeof(isr_stub_table) / sizeof(isr_stub_table[0]);

  for (uint8_t vector = 0; vector < isr_stub_len; vector++) {
    idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
    vectors[vector] = true;
  }

  __asm__ volatile("lidt %0" : : "m"(idtr)); // load the new IDT
  __asm__ volatile("sti");                   // set the interrupt flag
}
