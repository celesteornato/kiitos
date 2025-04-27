#ifndef FBIO_H_
#define FBIO_H_

#include <stddef.h>
#include <stdint.h>

struct PSF_font;

extern struct PSF_font *default_font;

/*
 ** Lowest-level API for printing a single character to the screen,
 ** fb designates a limine-like framebuffer array, akin to a pixel grid
 ** ppr designates the number of pixels per row (pitch * 8 / bpp)
 */
void k_putchar(char, uint32_t *fb, size_t ppr, uint64_t x, uint64_t y);

/*
 ** Lowest-level API for printing a string, works by repeatedly calling
 *k_puthchar. Will wrap back to off_x (off_y) when fbwidth (fbheight) is
 *reached.
 *
 ** If one wishes to print several strings through this function, they must be
 *the one to keep track of x and y.
 *
 ** If one wishes to use basic wrap-around behaviour
 *through multiple calls, they can simply increment x.
 *
 ** See k_putchar for details on the first parameters
 */
void k_puts(const char *, uint32_t *fb, size_t ppr, uint64_t x, uint64_t y,
            uint64_t off_x, uint64_t off_y, uint64_t fbwidth,
            uint64_t fbheight);

#endif // FBIO_H_
