#include <stdint.h>
// struct interrupt_descriptor {
//   uint16_t offset_low;
//   uint16_t seg_selector;
//   uint8_t ist: 3;
//   uint8_t reserved_low: 5;
//   uint8_t gate_type: 4;
//   uint8_t zero_bit: 1;
//   uint8_t dpl: 2;
//   uint8_t present: 1;
//   uint16_t mid_offset;
//   uint32_t offset_high;
//   uint32_t reserved;
// } __attribute__((packed));


struct interrupt_descriptor {
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t	    ist;          // The IST in the TSS that the CPU will load into RSP; set to zero for now
	uint8_t     attributes;   // Type and attributes; see the IDT page
	uint16_t    isr_mid;      // The higher 16 bits of the lower 32 bits of the ISR's address
	uint32_t    isr_high;     // The higher 32 bits of the ISR's address
	uint32_t    reserved;     // Set to zero
} __attribute__((packed));

struct idtr {
  uint16_t limit;
  uint64_t base;
} __attribute__((packed));

void idt_init(void);
