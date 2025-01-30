#include "gdt.h"
#include <stdint.h>

#define GDT_SIZE 3

struct gdt_descriptor
{
        uint16_t limit; // sizeof(gdt) - 1
        uint64_t base;
} __attribute__((packed));

struct segment_descriptor
{
        uint16_t limit_low;
        uint16_t base_low;
        uint8_t base_middle;
        uint8_t access;
        uint8_t limit_high : 4;
        uint8_t granularity : 4;
        uint8_t base_high;
} __attribute__((packed));

struct segment_descriptor g_gdt[GDT_SIZE];
struct gdt_descriptor g_gdtr;

void gdt_set_descriptor(int num, uint32_t base, uint32_t limit, uint8_t access,
                        uint8_t gran)
{
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

void gdt_init(void)
{

        // null desc
        gdt_set_descriptor(0, 0, 0, 0, 0);

        // 64 bit kernel code and data
        gdt_set_descriptor(1, 0, 0xFFFFF, 0x9B, 0xA);
        gdt_set_descriptor(2, 0, 0xFFFFF, 0x97, 0x8);

        g_gdtr = (struct gdt_descriptor){
            .limit = sizeof(g_gdt) - 1,
            .base = (uint64_t)g_gdt,
        };

        load_gdt(g_gdtr.limit, g_gdtr.base);
        reload_segments();
}
