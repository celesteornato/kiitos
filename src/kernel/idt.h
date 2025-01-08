#include <stdint.h>

struct interrupt_descriptor {
  uint16_t isr_low;         // 0 - 15
  uint16_t kernel_cs;       // 16 - 31
  uint8_t ist : 3;          // 32 - 34
  uint8_t reserved_low : 5; // 35 - 39
  uint8_t attributes;       // 40 - 47
  uint16_t isr_mid;         // 48 - 63
  uint32_t isr_high;        // 64 - 95
  uint32_t reserved_high;   // 96 - 127
} __attribute__((packed));

struct idtr {
  uint16_t limit;
  uint64_t base;
} __attribute__((packed));

void idt_init(void);
