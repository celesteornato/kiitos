#include "kiitstdio.h"
#include <stddef.h>
#include <stdint.h>

#define MONITOR_ADDR 0xB0000

#define PSF1_FONT_MAGIC 0x0436

typedef struct {
    uint16_t magic; // Magic bytes for identification.
    uint8_t fontMode; // PSF font mode.
    uint8_t characterSize; // PSF character size.
} PSF1_Header;


#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF_font;


void color_fbuff(struct limine_framebuffer* framebuffer, unsigned int color)
{
  for (size_t y = 0; y < framebuffer->height; ++y) {
    for (size_t x = 0; x < framebuffer->width; ++x) {
      volatile uint32_t *fb_ptr = framebuffer->address;
      fb_ptr[y * framebuffer->pitch / 4 + x] = color;
    }
  }
}
