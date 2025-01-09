#include "gdt.h"
#include <stdint.h>

#define GDT_SIZE 3

struct segment_descriptor g_gdt[GDT_SIZE];
struct gdt_descriptor g_gdtr;

void gdt_set_descriptor(int num, uint32_t base, uint32_t limit, uint8_t access,
                        uint8_t gran) {

  g_gdt[num] = (struct segment_descriptor){
      .base_low = base & 0xFFFF,
      .base_middle = (base >> 16) & 0xFF,
      .base_high = (base >> 24) & 0xFF,
      .limit_low = limit & 0xFFFF,
      .limit_high = limit >> 16,
      .granularity = gran,
      .access = access,
  };
}

void gdt_install() {

  // null desc
  gdt_set_descriptor(0, 0, 0, 0, 0);

  // 64 bit kernel code and data
  gdt_set_descriptor(1, 0, 0xFFFFF, 0b10011011, 0b1010);
  gdt_set_descriptor(2, 0, 0xFFFFF, 0b10010111, 0b1000);

  g_gdtr = (struct gdt_descriptor){
      .limit = sizeof(g_gdt) - 1,
      .base = (uint64_t)g_gdt,
  };

  load_gdt(g_gdtr.limit, g_gdtr.base);
  reload_segments();
}
