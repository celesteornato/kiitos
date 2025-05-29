#ifndef PAGING_H_
#define PAGING_H_
#include <stddef.h>
#include <stdint.h>

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
void map_page(size_t physaddr, size_t virtualaddr, size_t flags);
void limine_remap(const void *, size_t);
size_t get_physaddr(size_t virtualaddr);

#endif // PAGING_H_
