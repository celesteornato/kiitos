#include "kiitstdio.h"
#include "../kernel/kiitkio.h"
#include <stddef.h>
#include <stdint.h>

struct out out1;

#define PIXEL uint32_t
typedef struct {
  uint32_t magic;         /* magic bytes to identify PSF */
  uint32_t version;       /* zero */
  uint32_t headersize;    /* offset of bitmaps in file, 32 */
  uint32_t flags;         /* 0 if there's no unicode table */
  uint32_t numglyph;      /* number of glyphs */
  uint32_t bytesperglyph; /* size of each glyph */
  PIXEL height;           /* height in pixels */
  PIXEL width;            /* width in pixels */
} PSF_font;
static const char KB_LOOKUP[255] = {
    0,    0,    '1',  '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',  '=',
    '\b', '\t', 'q',  'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',  ']',
    '\n', 0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,    '\\', 'z',  'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,    '*',
    0,    ' ',  0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,
    0,    '7',  '8',  '9', '-', '4', '5', '6', '+', '1', '2', '3', '0',  '.'};

char handle_special_char(char c) {
  switch (c) {
  case '\n': {
    out1.cursor.x = 0;
    ++out1.cursor.y;
    break;
  }
  case '\r': {
    out1.cursor.x = 0;
    break;
  }
  case '\b': {
    --out1.cursor.x;
    break;
  }
  case '\t': {
    out1.cursor.x += 4;
    break;
  }
  case '\f': {
    out1.cursor.x = 0;
    out1.cursor.y = 0;
    kcolor_fbuff(out1.fb, g_stdbg);
    break;
  }
  default: {
    return 0;
  }
  }
  return 1;
}

void printc(char c) {
  if (handle_special_char(c))
    return;
  if (out1.cursor.x > out1.bounds.x) {
    ++out1.cursor.y;
    out1.cursor.x = 0;
  }
  if (out1.cursor.y > out1.bounds.y)
    out1.cursor.y = 0;
  k_putc(c, out1.cursor.x, out1.cursor.y, g_stdfg, g_stdbg, out1.fb);
  ++out1.cursor.x;
}

void prints(char *string) {
  char c;
  while ((c = *string) != 0) {
    printc(c);
    ++string;
  }
}

void printd(long num) {
  if (num > 10)
    printd(num / 10);
  printc((num % 10) + '0');
}

struct out out_new(struct limine_framebuffer *fb) {
  return (struct out){
      .cursor.x = 0,
      .cursor.y = 0,
      .fb = fb,
      .bounds.x = fb->width / default_font->width,
      .bounds.y = fb->height / default_font->height,
  };
}

unsigned char get_kb_key(void) { return KB_LOOKUP[inb(0x60)]; }
unsigned char get_kb_raw(void) { return inb(0x60); }
unsigned char kb_raw_to_key(unsigned char c) { return KB_LOOKUP[c]; }
