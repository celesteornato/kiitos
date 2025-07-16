#ifndef VMM_H_
#define VMM_H_

#include <stdint.h>
enum page_table_flags : uint64_t {
    PRESENT = 1 << 0,
    RDWR = 1 << 1,
    USER = 1 << 2,
    PWT = 1 << 3,
    PCD = 1 << 4,
    PAT = 1 << 7,
    GLOBAL = 1 << 8,
    NX = 1LL << 63,
};

void vmm_init(void);
void mmap(uintptr_t, void *);

#endif // VMM_H_
