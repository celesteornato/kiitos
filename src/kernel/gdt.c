#include "gdt.h"
#include <stdint.h>

#define GDT_SIZE 3

struct segment_descriptor g_gdt[GDT_SIZE];
struct gdt_descriptor g_gdtr;

void gdt_set_descriptor(int num, uint64_t base, uint64_t limit, uint8_t access,
                        uint8_t gran) {

  g_gdt[num] = (struct segment_descriptor){
      .base_low = (base & 0xFFFF),
      .base_middle = (base >> 16) & 0xFF,
      .base_high = (base >> 24) & 0xFF,
      .limit_low = (limit & 0xFFFF),
      .granularity = ((limit >> 16) & 0x0F) | (gran & 0xf0),
      .access = access,
  };
}

void gdt_install() {

  // null desc
  gdt_set_descriptor(0, 0, 0, 0, 0);

  // 64 bit code and data
  gdt_set_descriptor(1, 0, 0x0, 0b10011011, 0b10100000);
  gdt_set_descriptor(2, 0, 0x0, 0b10010111, 0b10000000);

  g_gdtr = (struct gdt_descriptor){
      .limit = (sizeof(struct segment_descriptor) * GDT_SIZE) - 1,
      .base = (uint64_t)g_gdt,
  };

  load_gdt(g_gdtr.limit, g_gdtr.base);
  reload_segments();
}
