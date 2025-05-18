#include <interrupts/devices/keyboard.h>
#include <io/keyboard.h>
#include <stdint.h>

static char kbdus[128] = {
    0,   27,  '1', '2', '3',  '4', '5', '6',  '7', '8', '9',  '0', '-', '=', '\b', '\t', 'q', 'w',
    'e', 'r', 't', 'y', 'u',  'i', 'o', 'p',  '[', ']', '\n', 0,   'a', 's', 'd',  'f',  'g', 'h',
    'j', 'k', 'l', ';', '\'', '`', 0,   '\\', 'z', 'x', 'c',  'v', 'b', 'n', 'm',  ',',  '.', '/',
    0,   '*', 0,   ' ', 0,    0,   0,   0,    0,   0,   0,    0,   0,   0,   0,    0,    0,   0,
    0,   0,   '-', 0,   0,    0,   '+', 0,    0,   0,   0,    0,   0,   0,   0,    0,    0,   0,
};
static char kbdfr[128] = {
    0,   27,  '&', 'e', '"', '\'', '(', '-', 'e', '_', 'c',  'a', ')', '=', '\b', '\t', 'a', 'z',
    'e', 'r', 't', 'y', 'u', 'i',  'o', 'p', '^', '$', '\n', 0,   'q', 's', 'd',  'f',  'g', 'h',
    'j', 'k', 'l', 'm', 'u', '*',  0,   '<', 'w', 'x', 'c',  'v', 'b', 'n', ',',  ';',  ':', '!',
    0,   '*', 0,   ' ', 0,   0,    0,   0,   0,   0,   0,    0,   0,   0,   0,    0,    0,   0,
    0,   0,   '-', 0,   0,   0,    '+', 0,   0,   0,   0,    0,   0,   0,   0,    0,    0,   0,
};
static const char *(layouts[]) = {[US] = kbdus, [FR] = kbdfr};
enum KB_LAYOUT current_layout = FR;

static inline bool is_printable(uint64_t key)
{
    return key <= 0xFF && (key & 128U) == 0 && layouts[current_layout][key] != 0;
}

char get_kb_char(void)
{

    // Per what our API states, we wait for a keypress AFTER the function is called
    // Thus, we will clear the last keypress and wait until it is set again.
    atomic_last_keypress = 0;

    // We ignore keypresses with a set bit on 0x80, as it indicates the key is lifted and not
    // pressed. non-printable keys are ignored.
    while (atomic_last_keypress == 0 || !is_printable(atomic_last_keypress))
    {
    }

    return layouts[current_layout][atomic_last_keypress];
}
