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
    DATA_SEG = 0, // Just to make reading easier
    DPL_0 = 1 << 13,
    DPL_1 = 1 << 14,
    USER = DPL_0 | DPL_1, // Who uses ring 1/2 anyways, amiright ?
    PRESENT = 1 << 15,
    LONG = 1 << 21,
    GRANULARITY = 1 << 23,
};

static struct cd_segment_descriptor gdt[5] = {};

struct [[gnu::packed]] {
    uint16_t limit;
    struct cd_segment_descriptor *base;
} gdtr = {.limit = sizeof(gdt) - 1, .base = gdt};

// See segment desc. definition for why we don't bother with most sections.
static void set_cd_descriptor(ptrdiff_t idx, uint32_t flags)
{
    constexpr int ONE_BIT = 1 << 12;

    // Everything else is ignored, so we can just set flags
    gdt[idx].flags = flags | ONE_BIT;
}

void gdt_init(void)
{
    set_cd_descriptor(0, 0);
    set_cd_descriptor(1, ACCESS | C_READ | CODE_SEG | PRESENT | LONG | GRANULARITY);
    set_cd_descriptor(2, ACCESS | D_WRITE | DATA_SEG | PRESENT | LONG | GRANULARITY);

    set_cd_descriptor(3, USER | ACCESS | C_READ | CODE_SEG | PRESENT | LONG | GRANULARITY);
    set_cd_descriptor(4, USER | ACCESS | D_WRITE | DATA_SEG | PRESENT | LONG | GRANULARITY);

    __asm__ volatile("lgdt %0" ::"m"(gdtr));
    asm_reload_segments();
}
