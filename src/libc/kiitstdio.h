#include <stdint.h>

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
  uint32_t fg;
  uint32_t bg;
};

static struct out global_out;

void printc(char c, struct out*);
void printd(long num, struct out*);
void printb(long num, struct out*);
void prints(char *string, struct out*);
unsigned char get_kb_key(void);
unsigned char get_kb_raw(void);

struct out out_new(struct limine_framebuffer *fb);
void set_global_out(struct limine_framebuffer *fb);

unsigned char kb_raw_to_key(unsigned char);
void gets(struct out *otp, char buffer[], uint64_t buff_size); 
