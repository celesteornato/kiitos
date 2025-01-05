#include "../limine.h"

static struct {
  struct {
    unsigned char x;
    unsigned char y;
  } cursor;
  struct limine_framebuffer *fb;
} g_stdout;

static uint32_t g_stdfg = 0xffffff;
static uint32_t g_stdbg = 0x000000;

void color_fbuff(struct limine_framebuffer *framebuffer, uint32_t color);

void k_putc(unsigned short int unicodecharacter, int start_x, int start_y, uint32_t fg,
            uint32_t bg, struct limine_framebuffer *framebuffer);

void k_puts(char *string, uint32_t start_x, uint32_t start_y, uint32_t fg, uint32_t bg,
            struct limine_framebuffer *fb);
void printc(char c);
void prints(char *string);
void init_stdout(unsigned char x, unsigned char y, struct limine_framebuffer*);
