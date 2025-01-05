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
extern char _binary_Solarize_12x29_psf_start[];
uint16_t *unicode;

void k_putc(unsigned short int c, int start_x, int start_y, uint32_t fg,
            uint32_t bg, struct limine_framebuffer *fb) {

  PSF_font *font = (PSF_font *)&_binary_Solarize_12x29_psf_start;
  unsigned char *glyph =
      (void *)font + font->headersize + c * font->bytesperglyph;

  int offset =
      (start_y * font->height * fb->pitch / 4) + (start_x * (font->width));

  int bytesperline = (font->width + 7) / 8;

  int line = offset;
  uint32_t y, x;
  PIXEL *fb_addr = fb->address;
  for (y = 0; y < font->height; y++) {
    line = offset;
    for (x = 0; x < font->width; x++) {
      fb_addr[line++] = ((glyph[x / 8] & (0x80 >> (x & 7))) ? fg : bg);
    }
    glyph += bytesperline;
    offset += fb->pitch / 4;
  }
}

void k_puts(char *string, uint32_t start_x, uint32_t start_y, uint32_t fg,
            uint32_t bg, struct limine_framebuffer *fb) {
  uint32_t x = start_x;
  uint32_t y = start_y;
  size_t ind = 0;
  while (string[ind] != 0) {
    if (y > 25)
      return;
    if (string[ind] == '\n' || x >= 80) {
      x = 0;
      y += 1;
      if (string[ind] == '\n')
        ++ind;
      continue;
    }
    k_putc(string[ind++], x++, y, fg, bg, fb);
  }
}

void k_putd(int num, uint32_t start_x, uint32_t start_y, uint32_t fg,
            uint32_t bg, struct limine_framebuffer *fb) {
  if (num == 0)
    k_putc('0', start_x, start_y, fg, bg, fb);
  if ((num / 10) == 0) {
    k_putc(num + '0', start_x, start_y, fg, bg, fb);
    return;
  }
  k_putd(num / 10, start_x, start_y, fg, bg, fb);
  k_putc((num % 10) + '0', start_x + 1, start_y, fg, bg, fb);
}

char handle_special_char(char c) {
  switch (c) {
  case '\n': {
    g_stdout.cursor.x = 0;
    ++g_stdout.cursor.y;
    break;
  }
  case '\r': {
    g_stdout.cursor.x = 0;
    break;
  }
  case '\b': {
    --g_stdout.cursor.x;
    break;
  }
  case '\t': {
    g_stdout.cursor.x += 4;
    break;
  }
  case '\f': {
    g_stdout.cursor.x = 0;
    g_stdout.cursor.y = 0;
    color_fbuff(g_stdout.fb, g_stdbg);
    break;
  }
    default: {return 0;}
  }
  return 1;
}

void printc(char c) {
  if (handle_special_char(c))
    return;
  if (g_stdout.cursor.x > 80) {
    ++g_stdout.cursor.y;
    g_stdout.cursor.x = 0;
  }
  if (g_stdout.cursor.x > 80) {
    ++g_stdout.cursor.y;
    g_stdout.cursor.x = 0;
  }
  if (g_stdout.cursor.y > 25)
    g_stdout.cursor.y = 0;
  k_putc(c, g_stdout.cursor.x, g_stdout.cursor.y, g_stdfg, g_stdbg,
         g_stdout.fb);
  ++g_stdout.cursor.x;
}

void prints(char *string) {
  char c;
  while ((c = *string) != 0) {
    printc(c);
    ++string;
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
void init_stdout(unsigned char x, unsigned char y,
                 struct limine_framebuffer *fb) {
  g_stdout.cursor.x = 0;
  g_stdout.cursor.y = 0;
  g_stdout.fb = fb;
}
