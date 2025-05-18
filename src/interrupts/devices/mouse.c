#include <basic/fbio.h>
#include <interrupts/pic.h>

#include <stdint.h>

void internal_mouse(void)
{
    k_puts("Hello mouse!!!");
    pic_send_eoi(12);
}
