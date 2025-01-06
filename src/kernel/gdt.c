#include "gdt.h"
#include <stdint.h>

struct segment_descriptor {
  uint16_t lower_limit;
  uint16_t lower_base;
  uint8_t mid_base;
  uint8_t access;
  uint8_t higher_limit : 4;
  uint8_t flags : 4;
  uint8_t higher_base;
} __attribute__((packed));

struct gdt_descriptor {
  uint16_t size; // sizeof(gdt) - 1
  struct segment_descriptor *offset;
} __attribute__((packed));

enum gdt_flags {
  GDT_FLAG_64BIT = 0x20,
  GDT_FLAG_32BIT = 0x40,
  GDT_FLAG_16BIT = 0x00,

  GDT_FLAG_GRANULARITY_1B = 0x00,
  GDT_FLAG_GRANULARITY_4K = 0x80
};

#define LOWER_BITS(bits) (bits & 0xffff)
#define MID_BITS(bits) ((bits >> 16) & 0xFF)
#define HIGH_BITS(bits) ((bits >> 16) & 0xFFFF)
#define GDT_HIGH_BITS(bits) ((bits >> 24) & 0xFF)
#define GDT_ENTRY(bs, lmt, access_byte, flgs)                                  \
  (struct segment_descriptor) {                                                \
    .lower_limit = LOWER_BITS(lmt), .lower_base = LOWER_BITS(bs),              \
    .mid_base = MID_BITS(bs), .access = access_byte,                           \
    .higher_limit = HIGH_BITS(lmt), .flags = flgs,                             \
    .higher_base = HIGH_BITS(bs)                                               \
  }

struct segment_descriptor g_gdt[] = {
    GDT_ENTRY(0, 0, 0, 0),            // Null descriptor
    GDT_ENTRY(0, 0xFFFFF, 0x9A, 0xA), // Kmode code segment
    GDT_ENTRY(0, 0xFFFFF, 0x92, 0xC), // Kmode data segment
    GDT_ENTRY(0, 0xFFFFF, 0xFA, 0xA), // Umode code segment
    GDT_ENTRY(0, 0xFFFFF, 0xF2, 0xC), // Umode data segment
};

struct gdt_descriptor g_gdt_descriptor = {.size = sizeof(g_gdt) - 1,
                                          .offset = g_gdt};

void load_gdt(void) {
  struct gdt_descriptor *gdtr = &g_gdt_descriptor;
  __asm__("mov [%0], %%di\nmov [%0+2], %%rsi\nlgdt [%0];":"=r"(gdtr));
}
