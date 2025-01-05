#include "kiitstdio.h"
#include <stddef.h>
#include <stdint.h>

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

#define PIXEL uint32_t
extern char *_binary_Solarize_12x29_psf_start;
uint16_t *unicode;

void k_putchar(unsigned short int c, int start_x, int start_y, uint32_t fg,
               uint32_t bg, struct limine_framebuffer *fb) {

  PSF_font *font = (PSF_font *)&_binary_Solarize_12x29_psf_start;

  unsigned char *glyph =
      (void *)font + font->headersize + c * font->bytesperglyph;

  int offset =
      (start_y * font->height * fb->pitch) + (start_x * (font->width + 1));

  int bytesperline = (font->width + 7) / 8;

  int line = offset;
  uint32_t y, x;
  for (y = 0; y < font->height; y++) {
    line = offset;
    for (x = 0; x < font->width; x++) {
      *((PIXEL *)(fb->address + line)) =
          glyph[x / 8] & (0x80 >> (x & 7)) ? fg : bg;

      line += sizeof(PIXEL);
    }
    glyph += bytesperline;
    offset += fb->pitch;
  }
}
void color_fbuff(struct limine_framebuffer *fb, uint32_t color) {
  for (size_t y = 0; y < fb->height; ++y) {
    for (size_t x = 0; x < fb->width; ++x) {
      volatile uint32_t *fb_ptr = fb->address;
      fb_ptr[y * fb->pitch / 4 + x] = color;
    }
  }
}
