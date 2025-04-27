#ifndef IDT_H_
#define IDT_H_

#define IDT_MAX_DESCRIPTORS 256
#define IDT_HW_DESCRIPTORS 0x20
/*
** Initialises the interrupt descriptor table, should not be used out of
*kmain()'s init sequence.
*/
void idt_init(void);

#endif // IDT_H_
