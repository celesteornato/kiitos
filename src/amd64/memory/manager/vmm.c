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
#include <stddef.h>
#include <stdint.h>
#include <string.h>

[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_memmap_request memmap_request = {.id = LIMINE_MEMMAP_REQUEST,
                                                               .revision = 0};
[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_kernel_address_request kern_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 0};

static bool is_vmm_initialised = false;

static void switch_cr3(uintptr_t *pml4)
{
    uintptr_t cr3 = hhdm_phys(pml4) | PTE_PRESENT | PTE_RDWR;
    __asm__ volatile("mov %0, %%cr3" ::"r"(cr3));
}
static void refresh_tlb(void)
{
    __asm__ volatile("mov %cr3, %rax; mov %rax, %cr3");
}

void vmm_init(void)
{
    if (is_vmm_initialised)
    {
        putsf("Attempt to re-init vmm after it has already been done!", COLOR, RED, D_BLUE);
        return;
    }
    constexpr uint64_t standard_flags = PTE_PRESENT | PTE_RDWR;
    uintptr_t *pml4 = hhdm_get_page();

    // We set up recursive mapping while we still have pml4.
    pml4[511] = hhdm_phys(pml4) | standard_flags;

    uintptr_t kern_add_p = kern_request.response->physical_base;
    uintptr_t kern_add_v = kern_request.response->virtual_base;

    struct limine_memmap_response *mmr = memmap_request.response;

    for (uint64_t i = 0; i < mmr->entry_count; ++i)
    {
        size_t len = (mmr->entries[i]->length / 4096) + 1;
        uintptr_t base = mmr->entries[i]->base;
        uint64_t type = mmr->entries[i]->type;

        switch (type)
        {
        case LIMINE_MEMMAP_KERNEL_AND_MODULES:
            hhdm_mmap_len(pml4, kern_add_p, kern_add_v, standard_flags, len);
            break;
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            // 4k-alignment is guaranteed so we can save one page here
            hhdm_mmap_len(pml4, base, (uintptr_t)hhdm_virt(base), PTE_PRESENT | PTE_RDWR, len - 1);
            break;
        case LIMINE_MEMMAP_RESERVED:    // Writing over Reserved causes a #PF
        case LIMINE_MEMMAP_FRAMEBUFFER: // We handle the FB on our own
            break;
        default:
            hhdm_mmap_len(pml4, base, (uintptr_t)hhdm_virt(base), standard_flags, len);
        }
    }

    uintptr_t fb_padd = hhdm_phys((void *)get_fb_address());
    hhdm_mmap_len(pml4, fb_padd, get_fb_address(), standard_flags, 1 + (get_fb_size() / 4096));
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
    }

    pml3[511] = pml4[pml4_idx];
    if (!(pml3[pml3_idx] & PTE_PRESENT))
    {
        if (pmm_alloc(&pml3[pml3_idx]) != PMM_OK)
        {
            goto error;
        }
        pml3[pml3_idx] |= PTE_PRESENT | flags;
    }

    pml2[511] = pml3[pml3_idx];
    if (!(pml2[pml2_idx] & PTE_PRESENT))
    {
        if (pmm_alloc(&pml2[pml2_idx]) != PMM_OK)
        {
            goto error;
        }
        pml2[pml2_idx] |= PTE_PRESENT | flags;
    }

    pml1[511] = pml2[pml2_idx];

    // Note the condition is not the same here
    if (pml1[pml1_idx] & PTE_PRESENT)
    {
        putsf("Trying to mmap to an already-existing vaddr!", COLOR, PURPLE, D_BLUE);
        // goto error;
    }

    pml1[pml1_idx] = physaddr_aligned | PTE_PRESENT | flags;
    refresh_tlb();
    return;

error:
    putsf("Encountered error allocating!", COLOR, RED, D_BLUE);
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
        putsf("PMM Free error @ munmap : % isn't pmm_alloc-ed", UNUM, 16, paddr);
        return;
    }
    pml1[pml1_idx] &= ~PTE_PRESENT;

    refresh_tlb();
}
