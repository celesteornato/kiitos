#include <basic/fbio.h>
#include <basic/kio.h>
#include <interrupts/pic.h>

#include <stdint.h>

void internal_mouse(void)
{
    k_dbg_puts("Hello mouse!!!");
    __asm__("int $99");
    pic_send_eoi(12);
}
