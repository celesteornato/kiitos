#include "arch/x86/asm_io.h"
#include <arch/x86/asm_functions.h>
#include <basic/fbio.h>
#include <io/gpiodefs.h>
#include <io/mouse.h>
#include <stdint.h>

static constexpr int MAX_RETRIES = 4;

void ps2_mouse_init(void)
{
    k_print("Todo: Make the mouse work\n", URG1);
    // Enable PS2 secondary port
    // outb(PS2_COMMAND, PS2_SECONDARY_ENABLE1);
    // outb(PS2_COMMAND, PS2_SECONDARY_ENABLE2);
    //
    // uint8_t status = inb(PS2_DATA) | 2U;
    //
    // outb(PS2_COMMAND, 0x60);
    // outb(PS2_DATA, status);
    //
    // int tries = 0;
    // volatile uint8_t answer = 0;
    // for (tries = 0; tries < MAX_RETRIES && answer != PS2_ACKNOWLEDGEMENT; ++tries)
    //{
    //
    // io_wait();
    // outb(PS2_COMMAND, 0xD4);
    //
    // io_wait();
    // outb(PS2_DATA, PS2_MOUSE_DATA_REPORT);
    // io_wait();
    //
    // answer = inb(PS2_DATA);
    // io_wait();
    //}
    //
    // if (answer == PS2_ACKNOWLEDGEMENT)
    //{
    // k_puts("\tPS/2 Mouse sucessfully initialised!");
    // k_print("\t\tOnly took ", 0);
    // k_printd(tries);
    // k_puts(tries == 1 ? " try." : " tries.");
    //}
    // else
    //{
    // k_print("\tError initialising PS/2 mouse:\n\terror code ", 0x2);
    // k_printd_base(answer, 16);
    // k_print("\n", 0);
    //}
}
