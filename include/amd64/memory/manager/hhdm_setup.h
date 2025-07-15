#ifndef HHDM_SETUP_H_
#define HHDM_SETUP_H_
#include <stddef.h>
#include <stdint.h>

/* This header file is meant to be used by amd64/memory/manager/vmm.c, it handles
 * part of the transition between limine's HHDM and our custom mappings */

/* Translates an virtaddress to its physical counterpart while the hhdm is active */
uintptr_t hhdm_phys(const void *addr);

/* While the hhdm is active, get a new zeroed page from the pmm */
void *hhdm_get_page(void);

/* While the hhdm is active, works as mmap. If you don't know what "pml4" should be when calling
 * this function, you probably should not be calling this function. */
void hhdm_mmap(uintptr_t pml4[static 1], uintptr_t physaddr, uintptr_t vaddr, uint64_t flags);

/* While the hhdm is active, works as a repeated mmap of contiguous v/p addresses. If you don't know
 * what "pml4" should be when calling this function, you probably should not be calling this
 * function. */
void hhdm_mmap_len(uintptr_t pml4[static 1], uintptr_t paddr, uintptr_t vaddr, size_t flags,
                   size_t length);

#endif // HHDM_SETUP_H_
