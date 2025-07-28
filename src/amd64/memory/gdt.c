#include <amd64/memory/gdt.h>
#include <stddef.h>
#include <stdint.h>

extern void asm_reload_segments(void);

/* Code/Data segment descriptor.
 * Everything but flags are ignored in long mode. Flags are 32-bit to adhere to the AMD manual's
 * convention.
 * */
struct [[gnu::packed]] cd_segment_descriptor {
    uint32_t ignored;
    uint32_t flags;
};

enum segdesc_flags {
    ACCESS = 1 << 8,
    C_READ = 1 << 9,
    D_WRITE = 1 << 9,
    CONFORMING = 1 << 10,
    CODE_SEG = 1 << 11,
    GDT_SEG = 1 << 12,
    DATA_SEG = 0, // Just to make reading easier
    DPL_0 = 1 << 13,
    DPL_1 = 1 << 14,
    USER = DPL_0 | DPL_1, // Who uses ring 1/2 anyways, amiright ?
    PRESENT = 1 << 15,
    LONG = 1 << 21,
    GRANULARITY = 1 << 23,
};

static constexpr struct cd_segment_descriptor gdt[] = {
    {0},
    {.flags = ACCESS | C_READ | CODE_SEG | GDT_SEG | PRESENT | LONG | GRANULARITY},
    {.flags = ACCESS | D_WRITE | DATA_SEG | GDT_SEG | PRESENT | LONG | GRANULARITY},
    {.flags = USER | ACCESS | C_READ | CODE_SEG | GDT_SEG | PRESENT | LONG | GRANULARITY},
    {.flags = USER | ACCESS | D_WRITE | DATA_SEG | GDT_SEG | PRESENT | LONG | GRANULARITY},
};

struct [[gnu::packed]] {
    uint16_t limit;
    const struct cd_segment_descriptor *base;
} gdtr = {.limit = sizeof(gdt) - 1, .base = gdt};

void gdt_init(void)
{
    __asm__ volatile("lgdt %0" ::"m"(gdtr));
    asm_reload_segments();
}
