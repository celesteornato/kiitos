#include "stdint.h"
/*
  made to work with Limine Revision 3 Framebuffers as a drop-in
*/
struct out {
  struct {
    unsigned char x;
    unsigned char y;
  } cursor;
  struct limine_framebuffer *fb;
  struct {
    unsigned char x;
    unsigned char y;
  } bounds;
};

static uint32_t g_stdfg = 0xffffff;
static uint32_t g_stdbg = 0x000000;

void printc(char c);
void printd(long num);
void prints(char *string);
unsigned char get_kb_key(void);
unsigned char get_kb_raw(void);
struct out out_new(struct limine_framebuffer *fb);
unsigned char kb_raw_to_key(unsigned char);
