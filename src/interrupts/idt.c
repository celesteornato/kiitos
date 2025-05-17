#include <interrupts/idt.h>
#include <interrupts/interrupts.h>
#include <stdbool.h>
#include <stdint.h>

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

static struct interrupt_descriptor idt[IDT_MAX_DESCRIPTORS] = {0};

static struct idtr idtr = {
    .base = idt,
    .limit = ((uint16_t)sizeof(struct interrupt_descriptor) * 256) - 1};

static bool vectors[IDT_MAX_DESCRIPTORS] = {0};

void (*(isr_table[]))(void) = {
    isr_0,     isr_1,     isr_2,  isr_3,    isr_4,  isr_5,  isr_6,  isr_7,
    isr_8,     isr_9,     isr_10, isr_11,   isr_12, isr_13, isr_14, isr_15,
    isr_16,    isr_17,    isr_18, isr_19,   isr_20, isr_21, isr_22, isr_23,
    isr_24,    isr_25,    isr_26, isr_27,   isr_28, isr_29, isr_30, isr_31,
    isr_clock, isr_kbinp, isr_34, isr_35,   isr_36, isr_37, isr_38, isr_40,
    isr_41,    isr_42,    isr_43, isr_mouse};

static void idt_set_descriptor(uint8_t vector, void (*isr)(void),
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
  const uint8_t isr_table_len = sizeof(isr_table) / sizeof(isr_table[0]);

  for (uint8_t vector = 0; vector < isr_table_len; vector++) {
    idt_set_descriptor(vector, isr_table[vector], 0x8E);
    vectors[vector] = true;
  }

  __asm__ volatile("lidt %0" : : "m"(idtr)); // load the new IDT
  // sti is called in the kmain function (main.c)
}
