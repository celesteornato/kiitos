#include <basic/fbio.h>
#include <esoterics/foomp.h>
#include <interrupts/syscall_values.h>
#include <io/keyboard.h>
#include <misc/art.h>
#include <misc/colours.h>
#include <stdint.h>

/*
** This is where the type system goes to die.
*/

static void fbwrite_intern(uint64_t opt1, uint64_t opt2, uint64_t opt3)
{
    switch (opt2)
    {
    case 0:
        k_print((char *)opt1, (uint32_t)opt3);
        break;
    case 0x1:
        k_putc((char)opt1);
        break;
    case 0x2:
        k_printd((int64_t)opt1);
        break;
    case 0x3:
        k_printd_base((int64_t)opt1, (uint8_t)opt3);
        break;
    default:
        k_print("Invalid FBWRITE syscall\n", 0x2);
    }
}

static void foomp_intern(uint64_t opt1, uint64_t opt2, uint64_t opt3, uint64_t opt4)
{
    if (opt1 * opt2 * opt3 * opt4 != 0xDEADBEEF)
    {
        // Invalid Foomp exception
        __asm__ volatile("int $26"); // NOLINT(hicpp-no-assembler)
    }

#pragma unroll
    for (uint32_t i = 0; i < 100; ++i)
    {
        k_paint(FOOMP_COLOR << i % 2);
    }
    fb_bg_color = FOOMP_COLOR;

    k_print(ASCII_FOOMP, 0x4);
}

/* Every type here is... debatable.
 * All of these are passed by registers, they're 64 bits because that will allow us
 * to put pretty much anything into our syscalls, and they're unsigned because the type
 * system literally does not exist in this function. Treat this as if it were B code.
 * */
uint64_t internal_syscall(uint8_t call_code, uint64_t opt1, uint64_t opt2, uint64_t opt3,
                          uint64_t opt4)
{
    switch (call_code)
    {
    case KBINP:
        return (uint64_t)get_kb_char();
    case FBWRITE:
        fbwrite_intern(opt1, opt2, opt3);
        return 0;
    case FOOMP:
        foomp_intern(opt1, opt2, opt3, opt4);
        return (uint64_t)foomp_counter;
    default:
        return (uint64_t)(-1);
    }
}
