#include "../limine.h"
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

extern struct PSF_font _binary_Solarize_12x29_psf_start;
extern struct PSF_font _binary_inconsolata_psf_start;
extern struct PSF_font _binary_powerline_font_psf_start;

void kcolor_fbuff(struct limine_framebuffer *framebuffer, uint32_t color);

void k_putc(unsigned short int unicodecharacter, int start_x, int start_y,
            uint32_t fg, uint32_t bg, struct limine_framebuffer *framebuffer);
void k_puts(char *string, uint32_t start_x, uint32_t start_y, uint32_t fg,
            uint32_t bg, struct limine_framebuffer *fb);

void k_setfont(struct PSF_font *);
struct PSF_font *k_getfont(void);

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void io_wait(void);
