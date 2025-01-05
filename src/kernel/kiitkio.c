#include "kiitkio.h"
#include <stddef.h>
#include <stdint.h>

static struct PSF_font *default_font = &_binary_powerline_font_psf_start;

inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  __asm__ volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}

inline void outb(uint16_t port, uint8_t val) {
  __asm__ volatile("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
  return;
}

void k_setfont(struct PSF_font* font)
{
  default_font = font;
}
struct PSF_font* k_getfont(void)
{
  return default_font;
}

void k_putc(unsigned short int c, int start_x, int start_y, uint32_t fg,
            uint32_t bg, struct limine_framebuffer *fb) {
  if (default_font->flags == 0 && c > 128)
    return;
  char *glyph = (char *)default_font + default_font->headersize +
                c * default_font->bytesperglyph;

  int offset = (start_y * default_font->height * fb->pitch / 4) +
               (start_x * (default_font->width));

  int bytesperline = (default_font->width + 7) / 8;

  int line = offset;
  uint32_t y, x;
  uint32_t *fb_addr = fb->address;
  for (y = 0; y < default_font->height; y++) {
    line = offset;
    for (x = 0; x < default_font->width; x++) {
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
    if (y > fb->height/default_font->height)
      return;
    if (string[ind] == '\n' || x > fb->width/default_font->width) {
      x = 0;
      y += 1;
      if (string[ind] == '\n')
        ++ind;
      continue;
    }
    k_putc(string[ind++], x++, y, fg, bg, fb);
  }
}

void kcolor_fbuff(struct limine_framebuffer *fb, uint32_t color) {
  for (size_t y = 0; y < fb->height; ++y) {
    for (size_t x = 0; x < fb->width; ++x) {
      volatile uint32_t *fb_ptr = fb->address;
      fb_ptr[y * fb->pitch / 4 + x] = color;
    }
  }
}
