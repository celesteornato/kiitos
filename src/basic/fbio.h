#ifndef FBIO_H_
#define FBIO_H_

#include <stddef.h>
#include <stdint.h>

struct PSF_font;

extern struct PSF_font *default_font;

void k_putchar(char, uint32_t *fb, size_t ppr, uint64_t x, uint64_t y);

void k_puts(const char *, uint32_t *fb, size_t ppr, uint64_t x, uint64_t y,
            uint64_t fbwidth, uint64_t fbheight);

#endif // FBIO_H_
