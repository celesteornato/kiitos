/*
 * For the entirety of this file, uintptr_t will mostly refer to a paging entry of the form
 * (physaddr << 12) | flags.
 */
#include "amd64/memory/manager/vmm.h"
#include "amd64/framebuffer/logging.h"
#include "amd64/memory/manager/hhdm_setup.h"
#include "amd64/memory/manager/pmm.h"
#include "fun/colors.h"
#include "limine.h"
#include "meta/ldsymbols.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static constexpr size_t PAGE_SIZE = 4096;

[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_memmap_request memmap_request = {.id = LIMINE_MEMMAP_REQUEST,
                                                               .revision = 0};
[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_kernel_address_request kern_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 0};

static bool is_vmm_initialised = false;

static inline void switch_cr3(uintptr_t *pml4)
{
    uintptr_t cr3 = hhdm_phys(pml4) | PTE_PRESENT | PTE_RDWR;
    __asm__ volatile("mov %0, %%cr3" ::"r"(cr3));
}
static inline void refresh_tlb(void)
{
    __asm__ volatile("mov %cr3, %rax; mov %rax, %cr3");
}

static void init_map_kernel_section(uintptr_t *pml4, uint8_t *section_start, uint8_t *section_end,
                                    ptrdiff_t offset, uint64_t flags)
{
    // The potential rounding-down is wanted, as a section that ends less than 4096 bytes after the
    // start of a page is still in that page anyways
    size_t len = (uintptr_t)(section_end - section_start) / 4096;

    hhdm_mmap_len(pml4, (uintptr_t)(section_start - offset), section_start, flags, len);
}

void vmm_init(void)
{
    if (is_vmm_initialised)
    {
        putsf("Attempt to re-init vmm after it has already been done!", LOG_COLOR, COLOR_RED,
              COLOR_D_BLUE);
        return;
    }
    constexpr uint64_t standard_flags = PTE_PRESENT | PTE_RDWR;
    uintptr_t *pml4 = hhdm_get_page();

    // We set up the recursive pagemap while we still have pml4 on hand.
    pml4[511] = hhdm_phys(pml4) | standard_flags;

    uintptr_t kern_add_p = kern_request.response->physical_base;
    ptrdiff_t kern_offset = &ld_kernel_start - (uint8_t *)kern_add_p;

    // We map the different sections of the kernel, text is the only one that isn't exec-protected
    init_map_kernel_section(pml4, &ld_text_start, &ld_text_end, kern_offset, standard_flags);
    init_map_kernel_section(pml4, &ld_data_start, &ld_data_end, kern_offset,
                            standard_flags | PTE_NX);
    init_map_kernel_section(pml4, &ld_limine_start, &ld_limine_end, kern_offset,
                            standard_flags | PTE_NX);

    putsf("kern spans from 0x% til 0x%", LOG_UNUM, 16, &ld_text_start, &ld_data_end);
    putsf("limine at       0x% til 0x%", LOG_UNUM, 16, &ld_limine_start, &ld_limine_end);
    putsf("fp spans from   0x% til 0x%", LOG_UNUM, 16, get_fb_address(),
          (char *)get_fb_address() + get_fb_size());

    struct limine_memmap_response *mmr = memmap_request.response;

    for (uint64_t i = 0; i < mmr->entry_count; ++i)
    {
        size_t len = (mmr->entries[i]->length / 4096);
        uintptr_t base = mmr->entries[i]->base;
        uint64_t type = mmr->entries[i]->type;
        switch (type)
        {
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            hhdm_mmap_len(pml4, base, hhdm_virt(base), standard_flags, len);
            putsf("Mapping brec at 0x% til 0x%", LOG_UNUM, 16, hhdm_virt(base),
                  (char *)hhdm_virt(base) + (len * 4096));
            break;
        default:
            break;
        }
    }

    uintptr_t fb_padd = hhdm_phys((void *)get_fb_address());
    hhdm_mmap_len(pml4, fb_padd, (void *)get_fb_address(), standard_flags, (get_fb_size() / 4096));
    switch_cr3(pml4);
}

void mmap(uintptr_t paddr, void *vaddr, uint64_t flags)
{
    uintptr_t physaddr_aligned = paddr - (paddr % 4096);

    uintptr_t vaddr_as_int = (uintptr_t)vaddr;
    size_t pml4_idx = (vaddr_as_int >> 39U) & 0x1FFU;
    size_t pml3_idx = (vaddr_as_int >> 30U) & 0x1FFU;
    size_t pml2_idx = (vaddr_as_int >> 21U) & 0x1FFU;
    size_t pml1_idx = (vaddr_as_int >> 12U) & 0x1FFU;

    /*These gets bitshifted to 12 to the left before being treated as addresses, to simplify the
     * masking process */
    constexpr size_t pml4_base = 0xFFFFFFFFFFFFFFFF;
    size_t pml3_base = (pml4_base << 9U) + (pml4_idx);
    size_t pml2_base = (pml3_base << 9U) + (pml3_idx);
    size_t pml1_base = (pml2_base << 9U) + (pml2_idx);

    uintptr_t *pml4 = (uintptr_t *)(pml4_base << 12U);
    uintptr_t *pml3 = (uintptr_t *)(pml3_base << 12U);
    uintptr_t *pml2 = (uintptr_t *)(pml2_base << 12U);
    uintptr_t *pml1 = (uintptr_t *)(pml1_base << 12U);

    if (!(pml4[pml4_idx] & PTE_PRESENT))
    {
        if (pmm_alloc(&pml4[pml4_idx]) != PMM_OK)
        {
            goto error;
        }
        pml4[pml4_idx] |= PTE_PRESENT | flags;
        memset(pml3, 0, PAGE_SIZE);
    }

    pml3[511] = pml4[pml4_idx];
    if (!(pml3[pml3_idx] & PTE_PRESENT))
    {
        if (pmm_alloc(&pml3[pml3_idx]) != PMM_OK)
        {
            goto error;
        }
        pml3[pml3_idx] |= PTE_PRESENT | flags;
        memset(pml2, 0, PAGE_SIZE);
    }

    pml2[511] = pml3[pml3_idx];
    if (!(pml2[pml2_idx] & PTE_PRESENT))
    {
        if (pmm_alloc(&pml2[pml2_idx]) != PMM_OK)
        {
            goto error;
        }
        pml2[pml2_idx] |= PTE_PRESENT | flags;
        memset(pml1, 0, PAGE_SIZE);
    }

    pml1[511] = pml2[pml2_idx];

    // Note the condition is not the same here
    if (pml1[pml1_idx] & PTE_PRESENT)
    {
        putsf("Trying to mmap to an already-existing vaddr!  PML1 of 0x% for vadd 0x%",
              LOG_COLOR | LOG_UNUM, COLOR_PURPLE, COLOR_D_BLUE, 16, pml1[pml1_idx], vaddr);
        putsf("indices: % % % %", LOG_UNUM, 16, pml1_idx, pml2_idx, pml3_idx, pml4_idx);
        goto error;
    }

    pml1[pml1_idx] = physaddr_aligned | PTE_PRESENT | flags;
    refresh_tlb();
    return;

error:
    putsf("Encountered error allocating!", LOG_COLOR, COLOR_RED, COLOR_D_BLUE);
    while (true)
    {
    }
}

void munmap(void *vaddr)
{
    uintptr_t vaddr_as_int = (uintptr_t)vaddr;
    size_t pml4_idx = (vaddr_as_int >> 39U) & 0x1FFU;
    size_t pml3_idx = (vaddr_as_int >> 30U) & 0x1FFU;
    size_t pml2_idx = (vaddr_as_int >> 21U) & 0x1FFU;
    size_t pml1_idx = (vaddr_as_int >> 12U) & 0x1FFU;

    /*These gets bitshifted to 12 to the left before being treated as addresses, to simplify the
     * masking process */
    constexpr size_t pml4_base = 0xFFFFFFFFFFFFFFFF;
    size_t pml3_base = (pml4_base << 9U) + (pml4_idx);
    size_t pml2_base = (pml3_base << 9U) + (pml3_idx);
    size_t pml1_base = (pml2_base << 9U) + (pml2_idx);
    uintptr_t *pml4 = (uintptr_t *)(pml4_base << 12U);
    uintptr_t *pml3 = (uintptr_t *)(pml3_base << 12U);
    uintptr_t *pml2 = (uintptr_t *)(pml2_base << 12U);
    uintptr_t *pml1 = (uintptr_t *)(pml1_base << 12U);

    if (!(pml4[pml4_idx] & PTE_PRESENT))
    {
        return;
    }
    if (!(pml3[pml3_idx] & PTE_PRESENT))
    {
        return;
    }
    if (!(pml2[pml2_idx] & PTE_PRESENT))
    {
        return;
    }
    if (!(pml1[pml1_idx] & PTE_PRESENT))
    {
        return;
    }

    uintptr_t paddr = pml1[pml1_idx] & ~0xFFFULL;
    if (pmm_free(paddr) != PMM_OK)
    {
        putsf("PMM Free error @ munmap : % isn't pmm_alloc-ed", LOG_UNUM, 16, paddr);
        return;
    }
    pml1[pml1_idx] = 0;

    refresh_tlb();
}
