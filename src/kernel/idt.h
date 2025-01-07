#include "stdint.h"
void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);
void load_idt(void);
