#include <arch/x86/asm_functions.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <io/gpiodefs.h>
#include <stdint.h>

static inline void conf_pic(uint16_t port, uint8_t byte)
{
    outb(port, byte);
    io_wait();
}

void pic_init(void)
{
    // starts the initialization sequence (in cascade mode)
    conf_pic(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    conf_pic(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    // ICW2: Master PIC vector offset
    conf_pic(PIC1_DATA, IDT_HW_DESCRIPTORS);

    // ICW2: Slave PIC vector offset
    conf_pic(PIC2_DATA, IDT_HW_DESCRIPTORS + 8);

    // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    conf_pic(PIC1_DATA, 4);
    // ICW3: tell Slave PIC its cascade identity (0000 0010)
    conf_pic(PIC2_DATA, 2);

    // ICW4: have the PICs use 8086 mode (and not 8080 mode)
    conf_pic(PIC1_DATA, ICW4_8086);
    conf_pic(PIC2_DATA, ICW4_8086);

    // Unmasks all interupts
    outb(PIC1_DATA, 0);
    outb(PIC2_DATA, 0);
}

void pic_send_eoi(uint8_t irq)
{
    if (irq >= 8)
    {
        outb(PIC2_COMMAND, PIC_EOI);
    }

    outb(PIC1_COMMAND, PIC_EOI);
}
