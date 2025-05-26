#ifndef PAGING_H_
#define PAGING_H_
#include <stddef.h>
#include <stdint.h>

union linear_address {
    size_t value;
    struct {
        size_t page_idx : 12;
        size_t pml1_idx : 9;
        size_t pml2_idx : 9;
        size_t pml3_idx : 9;
        size_t pml4_idx : 9;
    };
};

enum MAP_FLAGS : uint64_t {
    PRESENT = 0x1,
    RW = 0x2,
    USER = 0x4,
    WRITETHROUGH = 0x8,
    CACHEDISABLE = 0x10,
    ACCESSED = 0x20,
    DIRTY = 0x40,
    SIZE = 0x80,
    XD = 0x8000000000000000,
};

void paging_init(void);
void map_page(size_t physaddr, size_t virtualaddr, uint16_t flags);
void limine_remap(void *);

#endif // PAGING_H_
