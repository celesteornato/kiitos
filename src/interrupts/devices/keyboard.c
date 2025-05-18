#include <arch/x86/asm_functions.h>
#include <basic/fbio.h>
#include <interrupts/pic.h>
#include <io/gpiodefs.h>
#include <stdint.h>

// The biggest keypress is 6 bytes, this gives us leeway and makes it nicer converting the array
// into a single uint64
static uint8_t keypress_buffer[8] = {0};
static uint8_t keybuff_idx = 0;

/*
 Required to be atomic, as the operation assigning to it normally is not.
 At optimisations >=02, buff_to_keycode will write directly to this (which would not be atomic
 normally), creating a race condition with the other processes reading and writing to this value
 (i.e. io/keyboard).
*/
_Atomic uint64_t atomic_last_keypress = 0;

static uint64_t buff_to_keycode(void)
{
    union {
        uint8_t buff[8];
        uint64_t keycode;
    } conversion; // This is going to be endianness hell isn't it

    // Loop has to be reserved for big-endian platforms (really? hrmmmm)
    for (int i = 7; i >= 0; --i)
    {
        conversion.buff[i] = keypress_buffer[i];
        keypress_buffer[i] = 0;
    }

    keybuff_idx = 0;
    return conversion.keycode;
}

void internal_kbinp(void)
{
    uint8_t scancode = inb(PS2_PORT);

    keypress_buffer[keybuff_idx++] = scancode;

    // 0xE0 and 0xF0 indicate that another kb interrupt is coming, as some keys take several bytes
    if (scancode != 0xE0 && scancode != 0xF0)
    {
        atomic_last_keypress = buff_to_keycode();
    }

    pic_send_eoi(1);
}
