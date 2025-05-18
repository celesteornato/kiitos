#ifndef FBIO_H_
#define FBIO_H_

#include <stddef.h>
#include <stdint.h>

struct PSF_font;

extern const struct PSF_font *default_font;

enum PRINT_FLAGS : uint32_t {
    URG1 = 0x1,
    URG2 = 0x2,
};

/*
 ** Lowest-level API for printing a single character to the screen,
 ** fb designates a limine-like framebuffer array, akin to a pixel grid
 ** ppr designates the number of pixels per row (pitch * 8 / bpp)
 */
void k_internal_putc(char, volatile uint32_t *fb, size_t ppr, uint64_t x, uint64_t y);

/*
 ** Lowest-level API for printing a string, works by repeatedly calling
 k_puthchar. Will wrap back to off_x (off_y) when fbwidth (fbheight) is
 reached.

 ** If one wishes to print several strings through this function, they must be
 the one to keep track of x and y.

 ** If one wishes to use basic wrap-around behaviour
 through multiple calls, they can simply increment x.

 ** See k_putchar for details on the first parameters.
 */
void k_internal_puts(const char *, volatile uint32_t *fb, size_t ppr, uint64_t x, uint64_t y,
                     uint64_t off_x, uint64_t off_y, uint64_t fbwidth, uint64_t fbheight);

/*
** Called on init, stores info to simplify puts and putchar.
*/
void k_set_buff_settings(volatile uint32_t *fb, size_t ppr, uint64_t fbwidth, uint64_t fbheight);

/*
** Paints the framebuffer, takes a colour as its parameter.
*/
void k_paint(uint32_t);

/*
** Print, requires k_set_buff_settings to have been called at some point,
* provides an easier interface to print text.
** Flags:
* 0x1 and 0x2 determine level of intensity, bigger = more urgent.
*/
void k_print(const char *, uint32_t flags);

/*
** Wrapper around k_dbg_print, doesn't require flags and automatically goes to
*the next line.
*/
void k_puts(const char *);

/*
** Wrapper around k_dbg_print.
** See k_dbg_puts*/
void k_putc(char);

/*
** Wrapper around k_dbg_print, prints base 10 integers
** As it takes an int64_t, the behaviour for numbers >2^63 is undefined
*/
void k_printd(int64_t);

/* Same as k_dbg_printd, does so for any base between 2 and 37
 */
void k_printd_base(int64_t, uint8_t);

#endif // FBIO_H_
