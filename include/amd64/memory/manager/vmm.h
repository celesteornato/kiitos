#ifndef VMM_H_
#define VMM_H_

#include <stdint.h>
enum page_table_flags : uint64_t {
    PTE_PRESENT = 1 << 0,
    PTE_RDWR = 1 << 1,
    PTE_USER = 1 << 2,
    PTE_PWT = 1 << 3,
    PTE_PCD = 1 << 4,
    PTE_PAT = 1 << 7,
    PTE_GLOBAL = 1 << 8,
    PTE_NX = 1ULL << 63,
};

void vmm_init(void);
void mmap(uintptr_t, void *);

#endif // VMM_H_
