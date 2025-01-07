#include "gdt.h"
#include <stdint.h>

#define GDT_SIZE 5

struct segment_descriptor gdt[GDT_SIZE];
struct gdt_descriptor gdtr;

void gdt_set_descriptor(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
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
    gdtr = (struct gdt_descriptor){
        .limit = sizeof(gdt)-1,
        .base = (uint64_t)gdt,
    };

    gdt_set_descriptor(0, 0, 0, 0, 0);
    gdt_set_descriptor(1, 0, 0xFFFFF, 0x9A, 0xA);
    gdt_set_descriptor(2, 0, 0xFFFFF, 0x92, 0xC);
    load_gdt(&gdtr);
    reload_segments();
}
		
