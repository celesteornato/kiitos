#include "kiitstdio.h"
#include "../kernel/kiitkio.h"
#include "../libc/kiitklib.h"
#include <stddef.h>

#define PIXEL uint32_t
static const char KB_LOOKUP[255] = {
    0,    0,    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',  '=',
    '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',  ']',
    '\n', 0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,    '*',
    0,    ' ',  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,
    0,    '7',  '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0',  '.'};

static char pressed_keys[256] = {0};

char handle_special_char(char c, struct out *otp) {
  switch (c) {
  case '\n': {
    otp->cursor.x = 0;
    ++otp->cursor.y;
    break;
  }
  case '\r': {
    otp->cursor.x = 0;
    break;
  }
  case '\b': {
    --otp->cursor.x;
    printc(' ', otp);
    --otp->cursor.x;
    break;
  }
  case '\t': {
    otp->cursor.x += 4;
    break;
  }
  case '\f': {
    otp->cursor.x = 0;
    otp->cursor.y = 0;
    kcolor_fbuff(otp->fb, otp->bg);
    break;
  }
  default: {
    return 0;
  }
  }
  return 1;
}

void printc(char c, struct out *otp) {
  if (handle_special_char(c, otp))
    return;
  if (otp->cursor.x > otp->bounds.x) {
    ++otp->cursor.y;
    otp->cursor.x = 0;
  }
  if (otp->cursor.y > otp->bounds.y)
    otp->cursor.y = 0;
  k_putc(c, otp->cursor.x, otp->cursor.y, otp->fg, otp->bg, otp->fb);
  ++otp->cursor.x;
}

void prints(char *string, struct out *otp) {
  char c;
  while ((c = *string) != 0) {
    printc(c, otp);
    ++string;
  }
}

void printd(long num, struct out *otp) {
  if (num > 10)
    printd(num / 10, otp);
  printc((num % 10) + '0', otp);
}
void printb(long num, struct out *otp) {
  if (num > 1)
    printb(num / 2, otp);
  printc((num % 2) + '0', otp);
}

struct out out_new(struct limine_framebuffer *fb) {
  return (struct out){
      .cursor.x = 0,
      .cursor.y = 0,
      .fb = fb,
      .bounds.x = fb->width / k_getfont()->width - 1,
      .bounds.y = fb->height / k_getfont()->height - 1,
  };
}

unsigned char get_kb_key(void) { return KB_LOOKUP[inb(0x60)]; }
unsigned char get_kb_raw(void) { return inb(0x60); }
unsigned char kb_raw_to_key(unsigned char c) { return KB_LOOKUP[c]; }

void gets(struct out *otp, char buffer[], uint64_t buff_size) {
  unsigned char key, sc, abs_sc;
  unsigned long buffer_index = 0;
  memset(buffer, 0, buff_size);
  while (1) {
    if (buffer_index >= buff_size)
      return;
    sc = get_kb_raw();
    abs_sc = sc & ~128;
    key = kb_raw_to_key(abs_sc);

    if (key == 0)
      continue;

    if (sc == abs_sc && pressed_keys[abs_sc] == 0) {
      pressed_keys[abs_sc] = 1;
      if (sc == 0x1C)
        return;

      printc(key, otp);
      if (key == '\b')
        --buffer_index;
      else
        buffer[buffer_index++] = key;
      continue;
    }
    if (sc != abs_sc)
      pressed_keys[abs_sc] = 0;
  }
}
