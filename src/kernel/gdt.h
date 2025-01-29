#include <stdint.h>

struct segment_descriptor;
struct gdt_descriptor;
extern void load_gdt(uint16_t limit, uint64_t base);
extern void reload_segments(void);
void gdt_init(void);
