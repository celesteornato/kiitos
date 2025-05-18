#include <basic/fbio.h>
#include <basic/kio.h>
#include <io/gpiodefs.h>
#include <io/mouse.h>
#include <stdbool.h>
#include <stdint.h>

#define PS2_MOUSE_ENABLE 0xF4
#define PS2_MOUSE_ACKNOWLEDGEMENT 0xFA

#define MAX_RETRIES 4

void ps2_mouse_init(void)
{
    int tries = 0;
    uint8_t answer = 0;
    while (tries++ < MAX_RETRIES && answer != PS2_MOUSE_ACKNOWLEDGEMENT)
    {
        outb(PS2_PORT, PS2_MOUSE_ENABLE);
        io_wait();

        answer = inb(PS2_PORT);
        io_wait();
    }

    if (answer == PS2_MOUSE_ACKNOWLEDGEMENT)
    {
        k_dbg_puts("\tPS/2 Mouse sucessfully initialised!");
        k_dbg_print("\t\tOnly took ", 0);
        k_dbg_printd(tries);
        k_dbg_puts(" tries.");
    }
    else
    {
        k_dbg_print("\tError initialising PS/2 mouse:\n\terror code ", 0x2);
        k_dbg_printd_base(answer, 16);
        k_dbg_print("\n", 0);
    }
}
