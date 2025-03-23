#include "fbio.h"
#include <stdint.h>

struct PSF_font {
  uint32_t magic;         /* magic bytes to identify PSF */
  uint32_t version;       /* zero */
  uint32_t headersize;    /* offset of bitmaps in file, 32 */
  uint32_t flags;         /* 0 if there's no unicode table */
  uint32_t numglyph;      /* number of glyphs */
  uint32_t bytesperglyph; /* size of each glyph */
  uint32_t height;        /* height in pixels */
  uint32_t width;         /* width in pixels */
};

#define BG (0)
#define FG (0xffffff)
#define FGERR (0xff1111)
#define FGWR (0xff44ff)

extern struct PSF_font _binary_terminus_psf_start;
struct PSF_font *default_font = &_binary_terminus_psf_start;

void k_putchar(char c, uint32_t *fb, size_t ppr, uint64_t x, uint64_t y) {

  uint8_t *glyph_addr = (uint8_t *)default_font + default_font->headersize +
                        ((uint32_t)c * default_font->bytesperglyph);

  uint64_t offset =
      (y * default_font->height * ppr) + (x * default_font->width);

  int bytesperline = (default_font->width + 7) / 8;
  for (y = 0; y < default_font->height; ++y) {
    uint64_t line = offset;
    for (x = 0; x < default_font->width; ++x) {
      fb[line++] = (glyph_addr[x / 8] & (0x80 >> (x & 7))) ? FG : BG;
    }
    glyph_addr += bytesperline;
    offset += ppr;
  }
}

void k_puts(const char *s, uint32_t *fb, size_t ppr, uint64_t x, uint64_t y,
            uint64_t fbwidth, uint64_t fbheight) {
  const uint64_t max_col = fbwidth / default_font->width;
  const uint64_t max_lines = fbheight / default_font->height;
  for (; *s != '\0'; ++s, ++x) {
    uint64_t tmp_x = x % max_col;
    uint64_t tmp_y = (y + (x / max_col)) % max_lines;
    k_putchar(*s, fb, ppr, tmp_x, tmp_y);
  }
}
