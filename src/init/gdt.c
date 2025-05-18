#include <init/gdt.h>
#include <stdint.h>

#define GDT_SIZE 3

enum MASKS : uint32_t {
    LOWER_32 = 0xFFFF,
    LOWER_8 = 0xFF,
};

struct segment_descriptor {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t limit_high : 4;
    uint8_t flags : 4;
    uint8_t base_high;
} __attribute__((packed));

struct gdtr {
    uint16_t lim;
    struct segment_descriptor *addr;
} __attribute__((packed));

static struct segment_descriptor gdt[GDT_SIZE];
static const struct gdtr gdtr = {.lim = sizeof(gdt) - 1, .addr = gdt};

extern void reload_segments(void); // defined in lgdt.S

static void set_descriptor(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
    gdt[num] = (struct segment_descriptor){
        .base_low = base & LOWER_32,
        .base_mid = (base >> 16U) & LOWER_8,
        .base_high = (base >> 24U) & LOWER_8,
        .limit_low = limit & LOWER_32,
        .limit_high = (uint8_t)(limit >> 16U),
        .flags = flags,
        .access = access,
    };
}

void gdt_init(void)
{
    set_descriptor(0, 0, 0, 0, 0);

    set_descriptor(1, 0, 0xFFFFF, 0x9B, 0XA);
    set_descriptor(2, 0, 0xFFFFF, 0x97, 0X8);

    __asm__("lgdt %0" : : "m"(gdtr));
    reload_segments();
}
