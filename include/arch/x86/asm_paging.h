#ifndef ASM_PAGING_H_
#define ASM_PAGING_H_

#include <memory/paging.h>

// NOLINTBEGIN(hicpp-no-assembler)
static inline void change_cr3(struct cr3_pml4_descriptor pml4r)
{
    __asm__ volatile("mov %0, %%cr3" ::"r"(pml4r));
}
// NOLINTEND(hicpp-no-assembler)

#endif // ASM_PAGING_H_
