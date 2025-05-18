#include <arch/x86/asm_functions.h>
#include <basic/fbio.h>
#include <io/gpiodefs.h>
#include <io/mouse.h>
#include <stdint.h>

#define PS2_MOUSE_ENABLE 0xF4
#define PS2_MOUSE_ACKNOWLEDGEMENT 0xFA

#define MAX_RETRIES 4

void ps2_mouse_init(void)
{
    int tries = 0;
    volatile uint8_t answer = 0;
    for (tries = 0; tries < MAX_RETRIES && answer != PS2_MOUSE_ACKNOWLEDGEMENT; ++tries)
    {
        outb(PS2_PORT, PS2_MOUSE_ENABLE);
        io_wait();

        answer = inb(PS2_PORT);
        io_wait();
    }

    if (answer == PS2_MOUSE_ACKNOWLEDGEMENT)
    {
        k_puts("\tPS/2 Mouse sucessfully initialised!");
        k_print("\t\tOnly took ", 0);
        k_printd(tries);
        k_puts(tries == 1 ? " try." : " tries.");
    }
    else
    {
        k_print("\tError initialising PS/2 mouse:\n\terror code ", 0x2);
        k_printd_base(answer, 16);
        k_print("\n", 0);
    }
}
