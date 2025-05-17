#ifndef IDT_H_
#define IDT_H_

#define IDT_MAX_DESCRIPTORS 256

/* IDT_HW_DESCRIPTORS (idt.h) is the number of hardware interrupts, which
 * equates the offset at which regular ISRs should begin. The Slave PIC has an
 * offset of 8 compared to the Master PIC.*/
#define IDT_HW_DESCRIPTORS 0x20
/*
** Initialises the interrupt descriptor table, should not be used out of
*kmain()'s init sequence.
*/
void idt_init(void);

#endif // IDT_H_
