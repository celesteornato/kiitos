#include <stdint.h>

struct segment_descriptor
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)); 

struct gdt_descriptor {
  uint16_t limit; // sizeof(gdt) - 1
  uint64_t base;
} __attribute__((packed));

extern void load_gdt(uint16_t limit, uint64_t base);
extern void reload_segments(void);
void gdt_install(void);
