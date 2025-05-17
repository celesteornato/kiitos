#include <basic/fbio.h>
#include <basic/kio.h>
#include <interrupts/pic.h>

#include <stdint.h>

void internal_mouse(void)
{
    uint8_t bytes[3];
    bytes[0] = inb(0x60);
    bytes[1] = inb(0x60);
    bytes[2] = inb(0x60);
    k_dbg_puts("Hello mouse!!!");
    __asm__("int $0");
    pic_send_eoi(12);
}
