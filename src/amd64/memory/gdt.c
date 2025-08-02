#include <amd64/memory/gdt.h>
#include <stddef.h>
#include <stdint.h>

extern void asm_reload_segments(void);

/* Code/Data segment descriptor.
 * Everything but flags are ignored in long mode. Flags are 32-bit to adhere to the AMD manual's
 * convention.
 * */

enum segdesc_flags {
    SD_ACCESS = 1 << 8,
    SD_READ = 1 << 9,
    SD_WRITE = 1 << 9,
    SD_CONFORMING = 1 << 10,
    SD_CODE = 1 << 11,
    SD_GDTE = 1 << 12,
    SD_DATA = 0, // Just to make reading easier
    SD_DPL_0 = 1 << 13,
    SD_DPL_1 = 1 << 14,
    SD_USER = SD_DPL_0 | SD_DPL_1, // Who uses ring 1/2 anyways, amiright ?
    SD_PRESENT = 1 << 15,
    SD_LONG = 1 << 21,
    SD_GRAN = 1 << 23,
};

struct [[gnu::packed]] cd_segment_descriptor {
    uint32_t ignored;
    uint32_t flags;
};

static constexpr struct cd_segment_descriptor gdt[] = {
    {0},
    {.flags = SD_ACCESS | SD_READ | SD_CODE | SD_GDTE | SD_PRESENT | SD_LONG | SD_GRAN},
    {.flags = SD_ACCESS | SD_WRITE | SD_DATA | SD_GDTE | SD_PRESENT | SD_LONG | SD_GRAN},
    {.flags = SD_USER | SD_ACCESS | SD_READ | SD_CODE | SD_GDTE | SD_PRESENT | SD_LONG | SD_GRAN},
    {.flags = SD_USER | SD_ACCESS | SD_WRITE | SD_DATA | SD_GDTE | SD_PRESENT | SD_LONG | SD_GRAN},
};

/* C23 constexpr limitations do not allow for this to be a constexpr, if that is possible in the
 * future it's probably a good idea to do it */
static const struct [[gnu::packed]] {
    uint16_t limit;
    const struct cd_segment_descriptor *base;
} gdtr = {.limit = sizeof(gdt) - 1, .base = gdt};

void gdt_init(void)
{
    __asm__ volatile("lgdt %0" ::"m"(gdtr));
    asm_reload_segments();
}
