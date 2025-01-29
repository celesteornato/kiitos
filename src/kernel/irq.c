#include "kiitkio.h"
#include "../libc/kiitstdio.h"

#define IRQ_OFFSET 0x20

/* PIC command and data ports */
#define PIC1_CMD	0x20
#define PIC1_DATA	0x21
#define PIC2_CMD	0xa0
#define PIC2_DATA	0xa1

/* PIC initialization command word 1 bits */
#define ICW1_ICW4_NEEDED	(1 << 0)
#define ICW1_SINGLE			(1 << 1)
#define ICW1_INTERVAL4		(1 << 2)
#define ICW1_LEVEL			(1 << 3)
#define ICW1_INIT			(1 << 4)
/* PIC initialization command word 4 bits */
#define ICW4_8086			(1 << 0)
#define ICW4_AUTO_EOI		(1 << 1)
#define ICW4_BUF_SLAVE		(1 << 3) /* 1000 */
#define ICW4_BUF_MASTER		(3 << 2) /* 1100 */
#define ICW4_SPECIAL		(1 << 4)


void pic_init(void) {
   // send ICW1 saying we'll follow with ICW4 later on
	outb(ICW1_INIT | ICW1_ICW4_NEEDED, PIC1_CMD);
	outb(ICW1_INIT | ICW1_ICW4_NEEDED, PIC2_CMD);
	// send ICW2 with IRQ remapping
	outb(IRQ_OFFSET, PIC1_DATA);
	outb(IRQ_OFFSET + 8, PIC2_DATA);

	// send ICW3 to setup the master/slave relationship
	// ... set bit3 = 3rd interrupt input has a slave
	outb(4, PIC1_DATA);
	// ... set slave ID to 2
	outb(2, PIC2_DATA);
	// send ICW4 to set 8086 mode (no calls generated)
	outb(ICW4_8086, PIC1_DATA);
	outb(ICW4_8086, PIC2_DATA);
	// done, just reset the data port to 0
	outb(0, PIC1_DATA);
	outb(0, PIC2_DATA);   
}


void kb_interrupt(void)
{
   prints("a", &global_out);
}
