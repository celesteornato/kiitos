#include <basic/fbio.h>
#include <basic/kio.h>
#include <interrupts/pic.h>
#include <stdbool.h>
#include <stdint.h>

#define PS2_KB_RELEASED_BIT 128 // The bit that is on if the key is released

enum special_keys {
    LSHIFT = 0x2A,
    RSHIFT = 0x36,
    LCTRL = 0x14,
    RCTRL = 0x14E0,
};

static inline bool is_key_pressed(int64_t k) { return (k & PS2_KB_RELEASED_BIT) == 0; }
static inline bool key_eq(int64_t k, int64_t k2)
{
    return (k | PS2_KB_RELEASED_BIT) == (k2 | PS2_KB_RELEASED_BIT);
}

static char kbdus[128] = {
    0,   27,  '1', '2', '3',  '4', '5', '6',  '7', '8', '9',  '0', '-', '=', '\b', '\t', 'q', 'w',
    'e', 'r', 't', 'y', 'u',  'i', 'o', 'p',  '[', ']', '\n', 0,   'a', 's', 'd',  'f',  'g', 'h',
    'j', 'k', 'l', ';', '\'', '`', 0,   '\\', 'z', 'x', 'c',  'v', 'b', 'n', 'm',  ',',  '.', '/',
    0,   '*', 0,   ' ', 0,    0,   0,   0,    0,   0,   0,    0,   0,   0,   0,    0,    0,   0,
    0,   0,   '-', 0,   0,    0,   '+', 0,    0,   0,   0,    0,   0,   0,   0,    0,    0,   0,
};
static char kbdfr[256] = {
    0,   27,  '&', 'e', '"',  '\'', '(', '-',  'e', '_', 'c',  'a', ')', '=', '\b', '\t', 'a', 'z',
    'e', 'r', 't', 'y', 'u',  'i',  'o', 'p',  '[', ']', '\n', 0,   'q', 's', 'd',  'f',  'g', 'h',
    'j', 'k', 'l', 'm', '\'', '`',  0,   '\\', 'z', 'x', 'c',  'v', 'b', 'n', ',',  ';',  ':', '!',
    0,   '*', 0,   ' ', 0,    0,    0,   0,    0,   0,   0,    0,   0,   0,   0,    0,    0,   0,
    0,   0,   '-', 0,   0,    0,    '+', 0,    0,   0,   0,    0,   0,   0,   0,    0,    0,   0,
};
static char *current_kb = kbdfr;

static uint8_t keypress_buffer[8] = {0};
static uint8_t keybuff_idx = 0;

bool shift_pressed = false;
bool ctrl_pressed = false;

static int64_t buff_to_keycode(void)
{
    union {
        uint8_t buff[8];
        int64_t keycode;
    } conversion; // This is going to be endianness hell isn't it

    // Loop has to be reserved for big-endian platforms (really?)
    for (int i = 7; i >= 0; --i)
    {
        conversion.buff[i] = keypress_buffer[i];
        keypress_buffer[i] = 0;
    }
    keybuff_idx = 0;
    return conversion.keycode;
}

static void blurt_out_keybuff(void)
{
    int64_t keycode = buff_to_keycode(); // cleans keypress buffer for us.

    if (key_eq(keycode, RCTRL))
    {
        ctrl_pressed = (keycode & 128) == 0;
        return;
    }

    // "Simple" keypress
    if (keycode < 0xFF)
    {
        if (kbdus[keycode] == 'l' && ctrl_pressed)
        {
            k_paint(0);
        }
        if (is_key_pressed(keycode))
        {
            k_dbg_putc(kbdus[keycode]);
        }
        return;
    }

    k_dbg_print("0x", 0);
    k_dbg_printd_base(keycode, 16);
    k_dbg_putc('\n');

    switch (keycode)
    {
    default:
        break;
    }
}

void internal_kbinp(void)
{
    uint8_t scancode = inb(0x60);

    // keypress_buffer[keybuff_idx++] = scancode;

    if (scancode != 0xE0 && scancode != 0xF0)
    {
        // blurt_out_keybuff();
    }
    __asm__("int $35");

    pic_send_eoi(1);
}
