#include "gdt.h"
#include <stdint.h>

#define GDT_SIZE 7

struct segment_descriptor gdt[GDT_SIZE];
struct gdt_descriptor gdtr;

void gdt_set_descriptor(int num, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran)
{
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_install()
{

    // We need to match Limine's first  7 descriptors
    gdt_set_descriptor(0, 0, 0, 0, 0); // null desc

    // 16 bit code and data
    gdt_set_descriptor(1, 0, 0xFFFF, 0b10011011, 0b1000);
    gdt_set_descriptor(2, 0, 0xFFFF, 0b10010111, 0b1000);

    // 32 bit code and data
    gdt_set_descriptor(3, 0, 0xFFFFFFFF, 0b10011011, 0b1100);
    gdt_set_descriptor(4, 0, 0xFFFFFFFF, 0b10010111, 0b1100);

    // 64 bit code and data
    gdt_set_descriptor(5, 0, 0x0, 0b10011011, 0b1010);
    gdt_set_descriptor(6, 0, 0x0, 0b10010111, 0b1000);
    

    gdtr = (struct gdt_descriptor){
        .limit = (sizeof(struct segment_descriptor)*GDT_SIZE)-1,
        .base = (uint64_t)gdt,
    };

    load_gdt(gdtr.limit, gdtr.base);
    reload_segments();
}
		
